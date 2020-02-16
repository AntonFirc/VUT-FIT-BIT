----------------------------------------------------------------------------------
-- Author: Anton Firc (xfirca00)
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
USE ieee.numeric_std.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use work.lightsout_pack.ALL;


entity cell is
   GENERIC (
      MASK              : mask_t := (others => '1') -- mask_t definovano v baliku lightsout_pack
   );
   Port ( 
      INVERT_REQ_IN     : in   STD_LOGIC_VECTOR (3 downto 0);
      INVERT_REQ_OUT    : out  STD_LOGIC_VECTOR (3 downto 0) ;
      
      KEYS              : in   STD_LOGIC_VECTOR (4 downto 0);
      
      SELECT_REQ_IN     : in   STD_LOGIC_VECTOR (3 downto 0);
      SELECT_REQ_OUT    : out  STD_LOGIC_VECTOR (3 downto 0);
      
      INIT_ACTIVE       : in   STD_LOGIC;
      ACTIVE            : out  STD_LOGIC;
      
      INIT_SELECTED     : in   STD_LOGIC;
      SELECTED          : out  STD_LOGIC;

      CLK               : in   STD_LOGIC;
      RESET             : in   STD_LOGIC
   );
end cell;

architecture Behavioral of cell is
  constant IDX_TOP    : NATURAL := 0; -- index sousedni bunky nachazejici se nahore v signalech *_REQ_IN a *_REQ_OUT, index klavesy posun nahoru v signalu KEYS
                                      -- tzn. 1) pokud chci poslat kurzor sousedni bunce nahore, musim nastavit na jeden hodinovy takt SELECT_REQ_OUT(IDX_TOP) na '1'
                                      --      2) pokud plati, ze KEYS(IDX_TOP)='1', pak byla stisknuta klavesa nahoru
  constant IDX_LEFT   : NATURAL := 1; -- ... totez        ...                vlevo
  constant IDX_RIGHT  : NATURAL := 2; -- ... totez        ...                vpravo
  constant IDX_BOTTOM : NATURAL := 3; -- ... totez        ...                dole
  
  constant IDX_ENTER  : NATURAL := 4; -- index klavesy v KEYS, zpusobujici inverzi bunky (enter, klavesa 5)
  
  signal cur_active, cur_selected : std_logic := '0';
  signal select_out, invert_out : std_logic_vector (3 downto 0) := "0000";
begin

	process(CLK)
	begin
		if (CLK'event and CLK = '1') then
			
			if (RESET = '1') then
				cur_active <= INIT_ACTIVE;
				cur_selected <= INIT_SELECTED;
			end if; --RESET
			
			if(invert_out /= 0) then
				invert_out <= (others => '0');
			end if;
			
			if (cur_selected = '1') then
			
				if (KEYS(IDX_ENTER) = '1') then
				
						cur_active <= not cur_active;
						
						if (MASK.top = '1') then
							invert_out(IDX_TOP) <= '1';
						end if;
						
						if (MASK.left = '1') then
							invert_out(IDX_LEFT) <= '1';
						end if;	
					
						if (MASK.bottom = '1') then
							invert_out(IDX_BOTTOM) <= '1';
						end if;			
					
						if (MASK.right = '1') then
							invert_out(IDX_RIGHT) <= '1';
						end if;	
					
				end if;	--input ENTER
				
				if KEYS(IDX_TOP) = '1' then
					if MASK.top = '1' then
						select_out(IDX_TOP) <= '1';
						cur_selected <= '0';
					end if;
				end if;	--input TOP
					
				if KEYS(IDX_LEFT) = '1' then
					if MASK.left = '1' then
						select_out(IDX_LEFT) <= '1';
						cur_selected <= '0';
					end if;	
				end if;	--input LEFT
					
				if KEYS(IDX_BOTTOM) = '1' then
					if MASK.bottom = '1' then
						select_out(IDX_BOTTOM) <= '1';
						cur_selected <= '0';
					end if;	
				end if;	--input BOTTOM
				
				if KEYS(IDX_RIGHT) = '1' then
					if MASK.right = '1' then
						select_out(IDX_RIGHT) <= '1';
						cur_selected <= '0';
					end if;	
				end if;	--input RIGHT
					
			else 			-- end of if selected ; begin if not selected
			
				select_out <= (others => '0');
				invert_out <= (others => '0');
			
				if (INVERT_REQ_IN(IDX_TOP) = '1') then
					if (MASK.top = '1') then
						cur_active <= not cur_active;
					end if;
				end if;		--invert from TOP
				
				if (INVERT_REQ_IN(IDX_LEFT) = '1') then
					if (MASK.left = '1') then
						cur_active <= not cur_active;
					end if;
				end if;		--invert from LEFT
				
				if (INVERT_REQ_IN(IDX_BOTTOM) = '1') then
					if (MASK.bottom = '1') then
						cur_active <= not cur_active;
					end if;
				end if;	--invert from BOTTOM
				
				if (INVERT_REQ_IN(IDX_RIGHT) = '1') then
					if (MASK.right = '1') then
						cur_active <= not cur_active;
					end if;
				end if;	--invert from RIGHT
				
				if (SELECT_REQ_IN(IDX_TOP) = '1') then
					if (MASK.top = '1') then
						cur_selected <= '1';
					end if;
				end if;		--select from TOP
				
				if (SELECT_REQ_IN(IDX_LEFT) = '1') then
					if (MASK.left = '1') then
						cur_selected <= '1';
					end if;
				end if;		--selec from LEFT
				
				if (SELECT_REQ_IN(IDX_BOTTOM) = '1') then
					if (MASK.bottom = '1') then
						cur_selected <= '1';
					end if;
				end if;	--select from BOTTOM
				
				if (SELECT_REQ_IN(IDX_RIGHT) = '1') then
					if (MASK.right = '1') then
						cur_selected <= '1';
					end if;
				end if;	--select from RIGHT
			
			end if; --if selected or not
			 
		end if; --CLK event
	end process;
-- Pozadavky na funkci (sekvencni chovani vazane na vzestupnou hranu CLK)
--   pri resetu se nastavi ACTIVE a SELECTED na vychozi hodnotu danou signaly INIT_ACTIVE a INIT_SELECTED
--   pokud je bunka aktivni a prijde signal z klavesnice, tak se bud presune aktivita pomoci SELECT_REQ na dalsi bunky nebo se invertuje stav bunky a jejiho okoli pomoci INVERT_REQ (klavesa ENTER)
--   pokud bunka neni aktivni a prijde signal INVERT_REQ, invertuje svuj stav
--   pozadavky do okolnich bunek se posilaji a z okolnich bunek prijimaji, jen pokud je maska na prislusne pozici v '1'
	SELECT_REQ_OUT <= select_out;
	INVERT_REQ_OUT <= invert_out;
	ACTIVE <= cur_active;
	SELECTED <= cur_selected;
end Behavioral;

