
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity BCD is

	port(
			RESET, CLK, ENABLE : in std_logic;
			NUMBER1, NUMBER2, NUMBER3 :buffer std_logic_vector(3 downto 0)
		 );	

end BCD;

architecture Behavioral of BCD is
	signal num1,num2,num3 : std_logic_vector(3 downto 0):= "0000";
begin
	
	process(CLK, RESET)
		
		begin
			
			if RESET = '1' then
				num1 <= (others => '0');
				num2 <= (others => '0');
				num3 <= (others => '0');
			elsif (CLK'event and CLK = '1') then
				
				if ENABLE = '1' then
					if(num1 = 9) then 
						num1 <= (others => '0');
						if(num2 = 9) then 
							num2 <= (others => '0');
							if(num3 = 9) then
								num3 <= (others => '0');
							else
								num3 <= num3 + 1;
							end if;	--num3
						else
							num2 <= num2 + 1;
						end if;	--num2
					else
						num1 <= num1 + 1;
					end if;	--num1
					
				end if;	--enable
			
			end if;	--reset
	
	end process;			

NUMBER1 <= num1;
NUMBER2 <= num2;
NUMBER3 <= num3;

end Behavioral;

