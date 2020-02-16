library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( SMCLK: in std_logic;
		 RESET: in std_logic;
		 ROW: out std_logic_vector(0 to 7);
		 LED: out std_logic_vector(0 to 7)
);
end ledc8x8;

architecture main of ledc8x8 is

	signal row_led: std_logic_vector(0 to 7);
	signal row_actual: std_logic_vector(0 to 7) := "10000000";
    -- Sem doplnte definice vnitrnich signalu.

begin

    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.

	process(RESET,SMCLK)
	begin
	if (RESET = '1') then
		row_actual <= "10000000";
	elsif (SMCLK'event and SMCLK = '1') then
		case row_actual is
								when "10000000" => row_actual <= "01000000";
								when "01000000" => row_actual <= "00100000";
								when "00100000" => row_actual <= "00010000";
								when "00010000" => row_actual <= "00001000";
								when "00001000" => row_actual <= "00000100";
								when "00000100" => row_actual <= "00000010";
								when "00000010" => row_actual <= "00000001";
								when "00000001" => row_actual <= "10000000";
								when others => row_actual <= "00000000";
		end case;
	end if;		
	
	end process;
	
	process(row_actual)
	begin
		case row_actual is
								when "01000000" => row_led <= "10010000";
								when "00100000" => row_led <= "01100111";
								when "00010000" => row_led <= "01100001";
								when "00001000" => row_led <= "00000111";
								when "00000100" => row_led <= "01100111";
								when "00000010" => row_led <= "01100111";
								when others 	 => row_led <= "11111111";				
		end case;						
		
	end process;

	ROW <= row_actual;
	LED <= row_led;

end main;
