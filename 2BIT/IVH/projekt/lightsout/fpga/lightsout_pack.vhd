library IEEE;
use IEEE.STD_LOGIC_1164.all;

package lightsout_pack is

	type mask_t is record
		top, bottom, left, right : std_logic;
	end record;
						 
	function getmask(x, y: natural; COLUMNS, ROWS: natural) return mask_t;
	function vypocet(x,y:natural;offset:natural) return natural;

end lightsout_pack;

package body lightsout_pack is

	function getmask(x, y : natural; COLUMNS, ROWS : natural) return mask_t is
		
		variable maska : mask_t;
		
	begin
		
		if (y <= 0) then
			maska.top:= '0';
		else
			maska.top:= '1';
		end if;
		
		if (y >= ROWS-1) then 
			maska.bottom:= '0';
		else
			maska.bottom:= '1';
		end if;	
		
		if (x <= 0) then
			maska.left:= '0';
		else
			maska.left:='1';
		end if;	

		if	(x >= COLUMNS-1) then 
			maska.right:= '0';
		else
			maska.right:='1';
		end if;
		
		return maska;
		
	end function;	
	
	function vypocet(x,y:natural;offset:natural) return natural is
	
		variable tmp : natural;
	
	begin
	
	tmp := 4*((5*y)+x) + offset;
	return tmp;
	
	end function;
 
end lightsout_pack;
