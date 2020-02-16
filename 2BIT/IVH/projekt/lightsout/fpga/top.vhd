library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.vga_controller_cfg.all;
use work.lightsout_pack.all;

architecture main of tlv_pc_ifc is

	--VGA controls
	signal vga_mode  : std_logic_vector(60 downto 0); -- default 640x480x60

   signal irgb : std_logic_vector(8 downto 0);

   signal row : std_logic_vector(11 downto 0);
   signal col : std_logic_vector(11 downto 0);
	
	--Direction constants
	constant IDX_TOP    : NATURAL := 0; 
	constant IDX_LEFT   : NATURAL := 1; 
	constant IDX_RIGHT  : NATURAL := 2; 
	constant IDX_BOTTOM : NATURAL := 3; 
	constant IDX_ENTER  : NATURAL := 4; 
	
	--CELL Inputs
   signal KEYS : std_logic_vector(4 downto 0) := (others => '0');
   signal INIT_ACTIVE_X : std_logic_vector(24 downto 0) := "1101110101011101010111011";
   signal INIT_SELECTED_X : std_logic_vector(24 downto 0) := (others => '0');

 	--CELL Outputs
   signal INVERT_REQ_OUT_X : std_logic_vector(99 downto 0);
   signal SELECT_REQ_OUT_X : std_logic_vector(99 downto 0);
   signal ACTIVE_X : std_logic_vector(24 downto 0) := (others => '0');
   signal SELECTED_X : std_logic_vector(24 downto 0) := (others => '0');
	
	--KEYBOARD
	signal kbrd_data_out : std_logic_vector(15 downto 0);
	signal kbrd_data_vld : std_logic;

	signal difx,dify : std_logic_vector(11 downto 0);

   signal sx: integer range 0 to 639 := 164;
	signal sy: integer range 0 to 479 := 64;
	
	signal active : std_logic_vector(8 downto 0) := "111111001";
	signal non_active : std_logic_vector(8 downto 0) := "010010100";
	signal cell_state : std_logic_vector(224 downto 0);
	
	signal NUM1,NUM2,NUM3 : std_logic_vector(3 downto 0);
	signal ENABLE_CNT : std_logic;
	
	signal display_num : std_logic_vector(3 downto 0);-- := "1010";
   signal num_out : std_logic;
	
	shared variable prnt_num, num_1, num_2, num_3 : std_logic;
	
	signal RESET_CELL : std_logic;
	
begin

-- Nastaveni grafickeho rezimu (640x480, 60 Hz refresh)
   setmode(r640x480x60, vga_mode);
	
	vga: entity work.vga_controller(arch_vga_controller) 
      generic map (REQ_DELAY => 1)
      port map (
         CLK    => CLK, 
         RST    => RESET,
         ENABLE => '1',
         MODE   => vga_mode,

         DATA_RED    => irgb(8 downto 6),
         DATA_GREEN  => irgb(5 downto 3),
         DATA_BLUE   => irgb(2 downto 0),
         ADDR_COLUMN => col,
         ADDR_ROW    => row,

         VGA_RED   => RED_V,
         VGA_BLUE  => BLUE_V,
         VGA_GREEN => GREEN_V,
         VGA_HSYNC => HSYNC_V,
         VGA_VSYNC => VSYNC_V
      );
		
	kbrd_ctrl: entity work.keyboard_controller(arch_keyboard)
		--generic map (READ_INTERVAL => 1000000)
		port map (
			CLK => SMCLK,
			RST => RESET,

			DATA_OUT => kbrd_data_out(15 downto 0),
			DATA_VLD => kbrd_data_vld,
   
			KB_KIN   => KIN,
			KB_KOUT  => KOUT
		);
		
	BCD : entity work.bcd
		port map (
			RESET => RESET_CELL,
			CLK => CLK,
			ENABLE => ENABLE_CNT,
			NUMBER1 => NUM1,
			NUMBER2 => NUM2,
			NUMBER3 => NUM3
			);
	
	number_encoding : entity work.num_enc
	port map (
         ADDRESS => display_num,
         ROW => row(3 downto 0),
         COL => col(2 downto 0),
         DATA => num_out
      );
	
	cell_gen:
	for I in 0 to 24 generate	
		cellx: entity work.cell 
			GENERIC MAP ( MASK => getmask(I mod 5,I / 5,5,5) )
			PORT MAP (
				 INVERT_REQ_IN(IDX_TOP) => INVERT_REQ_OUT_X((vypocet(I mod 5,abs(I / 5 -1),IDX_BOTTOM)) mod 99),
				 INVERT_REQ_IN(IDX_BOTTOM) => INVERT_REQ_OUT_X((vypocet(I mod 5,abs(I / 5 +1),IDX_TOP)) mod 99),
				 INVERT_REQ_IN(IDX_LEFT) => INVERT_REQ_OUT_X((vypocet(abs(I mod 5 -1),I / 5,IDX_RIGHT)) mod 99),
				 INVERT_REQ_IN(IDX_RIGHT) => INVERT_REQ_OUT_X((vypocet(abs(I mod 5 +1),I / 5,IDX_LEFT)) mod 99),
				 INVERT_REQ_OUT => INVERT_REQ_OUT_X(vypocet(I mod 5,I / 5,IDX_BOTTOM) downto vypocet(I mod 5,I / 5,IDX_TOP)),
				 KEYS => KEYS,
				 SELECT_REQ_IN(IDX_TOP) => SELECT_REQ_OUT_X((vypocet(I mod 5,abs(I / 5 -1),IDX_BOTTOM)) mod 99),
				 SELECT_REQ_IN(IDX_BOTTOM) => SELECT_REQ_OUT_X((vypocet(I mod 5,abs(I / 5 +1),IDX_TOP)) mod 99),
				 SELECT_REQ_IN(IDX_LEFT) => SELECT_REQ_OUT_X((vypocet(abs(I mod 5 -1),I / 5,IDX_RIGHT)) mod 99),
				 SELECT_REQ_IN(IDX_RIGHT) => SELECT_REQ_OUT_X((vypocet(abs(I mod 5 +1),I / 5,IDX_LEFT)) mod 99),
				 SELECT_REQ_OUT => SELECT_REQ_OUT_X(vypocet(I mod 5,I / 5,IDX_BOTTOM) downto vypocet(I mod 5,I / 5,IDX_TOP)),
				 INIT_ACTIVE => INIT_ACTIVE_X(I),
				 ACTIVE => ACTIVE_X(I),
				 INIT_SELECTED => INIT_SELECTED_X(I),
				 SELECTED => SELECTED_X(I),
				 CLK => CLK,
				 RESET => RESET_CELL
			  );
	end generate;	

	INIT_SELECTED_X(12) <= '1';
	--INIT_ACTIVE_X(1) <= '1';
	--INIT_ACTIVE_X(3) <= '1';
	--INIT_ACTIVE_X(5) <= '1';
	--INIT_ACTIVE_X(21) <= '1';	
	--INIT_ACTIVE_X(24) <= '1';

	process
	
		variable in_use : std_logic := '0';
	
	begin
		
	if (CLK'event) and (CLK = '1') then
		
		KEYS(IDX_RIGHT) <= '0';
		KEYS(IDX_LEFT) <= '0';
		KEYS(IDX_TOP) <= '0';
		KEYS(IDX_BOTTOM) <= '0';
		KEYS(IDX_ENTER) <= '0'; 
		ENABLE_CNT <= '0';
		RESET_CELL <= RESET;
		
		if (in_use = '0') then
            if kbrd_data_vld='1' then 
               in_use:='1';
               if kbrd_data_out(9)='1' then  -- key 6
                  KEYS(IDX_RIGHT) <= '1';
               elsif kbrd_data_out(1)='1' then  -- key 4
                  KEYS(IDX_LEFT) <= '1';
               elsif kbrd_data_out(4)='1' then  -- key 2
                  KEYS(IDX_TOP) <= '1';
               elsif kbrd_data_out(6)='1' then  -- key 8
                  KEYS(IDX_BOTTOM) <= '1';
               elsif kbrd_data_out(5)='1' then     -- key 5
                  KEYS(IDX_ENTER) <= '1';
						ENABLE_CNT <= '1';
					elsif kbrd_data_out(12)='1' then     -- key A
                  INIT_ACTIVE_X <= "1101110101011101010111011";
						RESET_CELL <= '1';
					elsif kbrd_data_out(13)='1' then     -- key B
                  INIT_ACTIVE_X <= "1010110101010101101101010";
						RESET_CELL <= '1';
					elsif kbrd_data_out(14)='1' then     -- key C
                  INIT_ACTIVE_X <= "0011100000001110000000111";
						RESET_CELL <= '1';
					elsif kbrd_data_out(15)='1' then     -- key D
                  INIT_ACTIVE_X <= (others => '1');
						RESET_CELL <= '1';				
               end if;
					
            end if;
         else
            if (kbrd_data_vld = '0') then 
               in_use := '0';
            end if;
         end if;
			
			for I in 1 to 25 loop
					if(ACTIVE_X(I-1) = '1') then
						cell_state(9*I - 1 downto 9*I - 9) <= active;
					else
						cell_state(9*I - 1 downto 9*I - 9) <= non_active;
					end if;
			end loop;
			
			if num_1 = '1' then
            display_num <= NUM1;
         elsif num_2 = '1' then
            display_num <= NUM2;
			else 
				display_num <= NUM3;
         end if;
			
			if difx=0 or difx=314 then
            num_1 := '0';
            num_2 := '0';
				num_3 := '0';
         elsif difx=306 then
            num_1 := '1';
         elsif difx=298 then
            num_1 := '0';
            num_2 := '1';
			elsif difx=290 then
            num_2 := '0';
            num_3 := '1';	
         end if;
			
			if ((dify = 0) or (dify= 348)) then
            prnt_num := '0';
         elsif (dify = 331) then
            prnt_num := '1';
         end if;
									
	end if;	--CLK
	
	end process;

	difx <= col - conv_std_logic_vector(sx, col'length);
	dify <= row - conv_std_logic_vector(sy, col'length);

	irgb <= --1st row
			  "101001001" when (difx >= 10 and difx <54 ) and (dify >= 10 and dify <54 ) and (SELECTED_X(0) = '1') else	 	--cell 0
			  cell_state(8 downto 0) when (difx < 64) and (dify < 64) else
			  "101001001" when (difx >= 75 and difx <119 ) and (dify >= 10 and dify <54 ) and (SELECTED_X(1) = '1') else 	--cell 1
			  cell_state(17 downto 9) when (difx >= 65 and difx < 129) and (dify < 64) else								
			  "101001001" when (difx >= 140 and difx <184 ) and (dify >= 10 and dify <54 ) and (SELECTED_X(2) = '1') else	--cell 2
			  cell_state(26 downto 18) when (difx >= 130 and difx < 194) and (dify < 64) else	
			  "101001001" when (difx >= 205 and difx <249 ) and (dify >= 10 and dify <54 ) and (SELECTED_X(3) = '1') else	--cell 3
			  cell_state(35 downto 27) when (difx >= 195 and difx < 259) and (dify < 64) else							
			  "101001001" when (difx >= 270 and difx <314 ) and (dify >= 10 and dify <54 ) and (SELECTED_X(4) = '1') else	--cell 4	
			  cell_state(44 downto 36) when (difx >= 260 and difx < 324) and (dify < 64) else									  							
			  ---------------------------------------------------------------------------------------------------------------------------		
				--2nd row
			  "101001001" when (difx >= 10 and difx <54 ) and (dify >= 75 and dify <119 ) and (SELECTED_X(5) = '1') else	--cell 5
			  cell_state(53 downto 45) when (difx < 64) and (dify >= 65 and dify < 129) else								
			  "101001001" when (difx >= 75 and difx <119 ) and (dify >= 75 and dify <119 ) and (SELECTED_X(6) = '1') else	--cell 6
			  cell_state(62 downto 54) when (difx >= 65 and difx < 129) and (dify >= 65 and dify < 129) else 		
			  "101001001" when (difx >= 140 and difx <184 ) and (dify >= 75 and dify <119 ) and (SELECTED_X(7) = '1') else	--cell 7
			  cell_state(71 downto 63) when (difx >= 130 and difx < 194) and (dify >= 65 and dify < 129) else		
			  "101001001" when (difx >= 205 and difx <249 ) and (dify >= 75 and dify <119 ) and (SELECTED_X(8) = '1') else	--cell 8	
			  cell_state(80 downto 72) when (difx >= 195 and difx < 259) and (dify >= 65 and dify < 129) else		
			  "101001001" when (difx >= 270 and difx <314 ) and (dify >= 75 and dify <119 ) and (SELECTED_X(9) = '1') else	--cell 9		  
			  cell_state(89 downto 81) when (difx >= 260 and difx < 324) and (dify >= 65 and dify < 129) else		
			  ---------------------------------------------------------------------------------------------------------------------------
			  --3rd row
			  "101001001" when (difx >= 10 and difx <54 ) and (dify >= 140 and dify <184 ) and (SELECTED_X(10) = '1') else	--cell 10
			  cell_state(98 downto 90) when (difx < 64) and (dify >= 130 and dify < 194) else
			  "101001001" when (difx >= 75 and difx <119 ) and (dify >= 140 and dify <184 ) and (SELECTED_X(11) = '1') else			  
			  cell_state(107 downto 99) when (difx >= 65 and difx < 129) and (dify >= 130 and dify < 194) else		--cell 11
			  "101001001" when (difx >= 140 and difx <184 ) and (dify >= 140 and dify <184 ) and (SELECTED_X(12) = '1') else
			  cell_state(116 downto 108) when (difx >= 130 and difx < 194) and (dify >= 130 and dify < 194) else	--cell 12
			  "101001001" when (difx >= 205 and difx <249 ) and (dify >= 140 and dify <184 ) and (SELECTED_X(13) = '1') else
			  cell_state(125 downto 117) when (difx >= 195 and difx < 259) and (dify >= 130 and dify < 194) else	--cell 13
			  "101001001" when (difx >= 270 and difx <314 ) and (dify >= 140 and dify <184 ) and (SELECTED_X(14) = '1') else			  
			  cell_state(134 downto 126) when (difx >= 260 and difx < 324) and (dify >= 130 and dify < 194) else	--cell 14
			  --------------------------------------------------------------------------------------------------------------------------
			  --4th row
			  "101001001" when (difx >= 10 and difx <54 ) and (dify >= 205 and dify <249 ) and (SELECTED_X(15) = '1') else
			  cell_state(143 downto 135) when (difx < 64) and (dify >= 195 and dify < 259) else							--cell 15
			  "101001001" when (difx >= 75 and difx <119 ) and (dify >= 205 and dify <249 ) and (SELECTED_X(16) = '1') else
			  cell_state(152 downto 144) when (difx >= 65 and difx < 129) and (dify >= 195 and dify < 259) else	--cell 16
			  "101001001" when (difx >= 140 and difx <184 ) and (dify >= 205 and dify <249 ) and (SELECTED_X(17) = '1') else
			  cell_state(161 downto 153) when (difx >= 130 and difx < 194) and (dify >= 195 and dify < 259) else	--cell 17
			  "101001001" when (difx >= 205 and difx <249 ) and (dify >= 205 and dify <249 ) and (SELECTED_X(18) = '1') else
			  cell_state(170 downto 162) when (difx >= 195 and difx < 259) and (dify >= 195 and dify < 259) else	--cell 18
			  "101001001" when (difx >= 270 and difx <314 ) and (dify >= 205 and dify <249 ) and (SELECTED_X(19) = '1') else
			  cell_state(179 downto 171) when (difx >= 260 and difx < 324) and (dify >= 195 and dify < 259) else	--cell 19			  
			  ---------------------------------------------------------------------------------------------------------------------------
			  --5th row
			  "101001001" when (difx >= 10 and difx <54 ) and (dify >= 270 and dify <314 ) and (SELECTED_X(20) = '1') else
			  cell_state(188 downto 180) when (difx < 64) and (dify >= 260 and dify < 324) else							--cell 20	
			  "101001001" when (difx >= 75 and difx <119 ) and (dify >= 270 and dify <314 ) and (SELECTED_X(21) = '1') else
			  cell_state(197 downto 189) when (difx >= 65 and difx < 129) and (dify >= 260 and dify < 324) else	--cell 21
			  "101001001" when (difx >= 140 and difx <184 ) and (dify >= 270 and dify <314 ) and (SELECTED_X(22) = '1') else			  
			  cell_state(206 downto 198) when (difx >= 130 and difx < 194) and (dify >= 260 and dify < 324) else	--cell 22
			  "101001001" when (difx >= 205 and difx <249 ) and (dify >= 270 and dify <314 ) and (SELECTED_X(23) = '1') else
			  cell_state(215 downto 207) when (difx >= 195 and difx < 259) and (dify >= 260 and dify < 324) else	--cell 23
			  "101001001" when (difx >= 270 and difx <314 ) and (dify >= 270 and dify <314 ) and (SELECTED_X(24) = '1') else
			  cell_state(224 downto 216) when (difx >= 260 and difx < 324) and (dify >= 260 and dify < 324) else	--cell 24
			  ---------------------------------------------------------------------------------------------------------------------------
			  --Counter space
			  "111111111" when (num_out = '1') and (prnt_num = '1') and ((num_1 = '1') or (num_2 = '1') or (num_3 = '1')) else
           "000000000";

end main;

