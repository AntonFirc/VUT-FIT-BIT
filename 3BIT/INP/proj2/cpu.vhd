-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2017 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xfirca00
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='0') / zapis do pameti (DATA_RDWR='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is


signal PC_addr:	std_logic_vector(11 downto 0);
signal PC_inc :	std_logic;
signal PC_dec : 	std_logic;

signal ptr_addr: std_logic_vector(9 downto 0);
signal ptr_inc : std_logic;
signal ptr_dec : std_logic;

signal cell_val_inc : std_logic;
signal cell_val_dec : std_logic;

----------------------------------------------------------------
------------------Stavy konecneho automatu----------------------
----------------------------------------------------------------
type fsm_state is (
idle, fetch,	-- inicializace CPU
decode, 					-- dekodovani instrukce
state_val_inc, state_val_dec, state_val_inc_exec, state_val_dec_exec,				-- praca s hodnotou ukazatela
state_ptr_inc,  state_ptr_dec,    -- praca s ukazatelom
state_read, state_read_exec, 	-- vstup uzivatela
state_print, state_print_exec,	-- vystup na LCD 
while_begin_cond, while_begin, while_end, while_end_cond, while_skip, while_return,	--cykly
state_continue, -- continue ~
state_halt, state_nop			-- koniec, nic
);

signal pstate : fsm_state;
signal nstate : fsm_state;

----------------------------------------------------------------
------------------------Instrukcie------------------------------
----------------------------------------------------------------
type instruction_type is (
cell_v_inc, cell_v_dec,
ptr_addr_inc, ptr_addr_dec,
instr_while_b, instr_while_e,
print, get_char, instr_continue,
halt, nop
);
signal instruction : instruction_type;

begin

--ovladanie program countera
prog_counter: process (CLK, RESET)
begin
	if (RESET = '1') then
		PC_addr <= (others => '0');
	elsif (CLK'event) and (CLK='1') then
		if (PC_inc = '1') then
			PC_addr <= PC_addr + 1;
		elsif (PC_dec = '1') then
			PC_addr <= PC_addr - 1;
		end if;
	end if;
end process;

--praca s ukazatelom do RAM
ptr_move : process (CLK, RESET)
begin
	if (RESET = '1') then
		ptr_addr <= (others => '0');
	elsif (CLK'event) and (CLK='1') then
		if (ptr_inc = '1') then
			ptr_addr <= ptr_addr + 1;
		elsif (ptr_dec = '1') then
			ptr_addr <= ptr_addr - 1;
		end if;
	end if;
end process;

--Praca s bunkou v RAM
cell_change : process (CLK, RESET)
begin
	if (RESET = '1') then
		
	elsif (CLK'event) and (CLK='1') then
		if (cell_val_inc = '1') then
			DATA_WDATA <= DATA_RDATA + 1;
		elsif (cell_val_dec = '1') then
			DATA_WDATA <= DATA_RDATA - 1;
		elsif (pstate = state_read_exec) then
			DATA_WDATA <= IN_DATA;
		else 
			DATA_WDATA <= X"00";
		end if;
	end if;
end process;

-----------------------------------------
---------dekodovanie instrukci-----------
-----------------------------------------
decode_instr: process (CODE_DATA)
begin
	case (CODE_DATA) is
		when X"3E" => instruction <= ptr_addr_inc; -->
		when X"3C" => instruction <= ptr_addr_dec; --<
		when X"2B" => instruction <= cell_v_inc; --+
		when X"2D" => instruction <= cell_v_dec; ---
		when X"5B" => instruction <= instr_while_b; --[
		when X"5D" => instruction <= instr_while_e; --]
		when X"2E" => instruction <= print; --.
		when X"2C" => instruction <= get_char; --,
		when X"7E" => instruction <= instr_continue; -- ~
		when X"00" => instruction <= halt; --null
		when others=> instruction <= nop;
	end case;
end process;

---------------------------
---Prechod na dalsi stav---
---------------------------
fsm_pstate: process(RESET, CLK)
begin
	if (RESET = '1') then
		pstate <= idle;
	elsif (CLK'event) and (CLK = '1') then
		if (EN = '1') then
			pstate <= nstate;
		end if;
	end if;
end process;
--------------------------------
----Nastavenie dalsieho stavu---
--------------------------------
fsm_nstate: process (CLK, RESET)
begin
	--nastavi vychodzie hodnoty
	CODE_EN <= '0';
	DATA_RDWR <= '0';
	DATA_EN <= '0';
	IN_REQ <= '0';
	OUT_WE <= '0';
	PC_inc <= '0';
	PC_dec <= '0';
	ptr_inc <= '0';
	ptr_dec <= '0';
	cell_val_inc <= '0';
	cell_val_dec <= '0';
	
	case pstate is
		---------IDLE----------
		when idle =>
			nstate <= fetch;
		---------FETCH---------
		when fetch =>
			nstate <= decode;
			DATA_EN <= '1';
			CODE_EN <= '1';
		---------DECODE--------
		when decode =>
			case instruction is
				--zvysit hodnotu			
				when cell_v_inc =>	
					nstate <= state_val_inc;
				--znizit hodnotu
				when cell_v_dec =>
					nstate <= state_val_dec;
				--adresa ++
				when ptr_addr_inc =>
					nstate <= state_ptr_inc;	
				--adresa--
				when ptr_addr_dec =>
					nstate <= state_ptr_dec;			
				when print =>
					nstate <= state_print;		
				when get_char =>
					nstate <= state_read;					
				when instr_while_b =>
					nstate <= while_begin;					
				when instr_while_e =>
					nstate <= while_end;
				when instr_continue =>
					nstate <= state_continue;
				--HALT
				when halt =>
					nstate <= state_halt;
				--nic
				when others =>
					nstate <= state_nop;
			end case;	
		------instrukcia plus -----------
		when state_val_inc =>
			DATA_RDWR <= '0';
			cell_val_inc <= '1';			
			DATA_EN <= '1';
			nstate <= state_val_inc_exec;
		when state_val_inc_exec =>
			DATA_RDWR <= '1';		
			DATA_EN <= '1';
			nstate <= fetch;
			PC_inc <= '1';
		------instrukcia minus -----------
		when state_val_dec =>
			DATA_RDWR <= '0';
			cell_val_dec <= '1';
			DATA_EN <= '1';
			nstate <= state_val_dec_exec;		
		when state_val_dec_exec =>
			DATA_RDWR <= '1';		
			DATA_EN <= '1';
			nstate <= fetch;
			PC_inc <= '1';
		-------instrukcia > -------
		when 	state_ptr_inc =>
			ptr_inc <= '1';
			PC_inc <= '1';
			nstate <= fetch;
		-------instruckia < -------
		when state_ptr_dec =>
			ptr_dec <= '1';
			PC_inc <= '1';
			nstate <= fetch;
		-----instrukcia . -------------
		when state_print =>
			DATA_RDWR <= '0';
			DATA_EN <= '1';
			nstate <= state_print_exec;
		when state_print_exec =>
			if (OUT_BUSY = '0') then
				OUT_WE <= '1';
				OUT_DATA <= DATA_RDATA;
				PC_inc <= '1';
				nstate <= fetch;
			else
				nstate <= state_print_exec;
			end if;
		------instrukcia , ---------
		when state_read =>
			IN_REQ <= '1';
			nstate <= state_read_exec;
		when state_read_exec =>
			IN_REQ <= '1';
			if (IN_VLD = '1') then
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				PC_inc <= '1';
				nstate <= fetch;
			else
				nstate <= state_read_exec;
			end if;	
		-----instrukcia [ ------------	
		when while_begin =>
			DATA_RDWR <= '0';
			DATA_EN <= '1';
			nstate <= while_begin_cond;
			PC_inc <= '1';
		when while_begin_cond =>
			nstate <= fetch;
			if (DATA_RDATA = "00000000") then
				nstate <= while_skip;
			end if;
		when while_skip =>
			nstate <= while_skip;
			CODE_EN <= '1';
			if instruction = instr_while_e then
				nstate <= fetch;
			end if;
			PC_inc <= '1';
		-----instrukcia ]	--------------
		when while_end =>
			DATA_RDWR <= '0';
			DATA_EN <= '1';
			nstate <= while_end_cond;
		when while_end_cond =>
			nstate <= fetch;
			if (DATA_RDATA /= "00000000") then
				nstate <= while_return;
			else
				PC_inc <= '1';
			end if;
		when while_return =>
			nstate <= while_return;
			CODE_EN <= '1';
			if instruction = instr_while_b then
				nstate <= fetch;
				PC_inc <= '1';
			else 
				PC_dec <= '1';
			end if;
		-----instruckia ~------------		
		when state_continue =>	
			nstate <= state_continue;
			CODE_EN <= '1';
			if instruction = instr_while_e then
				nstate <= fetch;
			end if;
			PC_inc <= '1';
		----HALT-----------
		when state_halt =>
			nstate <= state_halt;
		-----NOP---------
		when state_nop =>
			PC_inc <= '1';
			nstate <= fetch;
		when others =>
	end case;	
	
end process;

CODE_ADDR <= PC_addr;
DATA_ADDR <= ptr_addr;

end behavioral;
 
