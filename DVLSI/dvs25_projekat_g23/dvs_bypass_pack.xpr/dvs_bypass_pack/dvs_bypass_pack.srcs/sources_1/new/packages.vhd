library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- arr types 
package ARR_TYPES is 
    type arr16bit is array (natural range <>) of std_logic_vector(15 downto 0);
    type arr8bit is array (natural range <>) of std_logic_vector(7 downto 0);
end package ARR_TYPES; 

package body ARR_TYPES is
end package body ARR_TYPES;   

-- imgW, imgH 
package CONSTANTS is
    constant C_MAX_IMG_W     : integer := 1024; -- smanji se za bonus 2 
    constant C_MAX_IMG_H     : integer := 1024; -- smanji se za bonus 2 
    
    constant MAX_WIN         : integer := 9; 

    constant PIPELINE_DELAY  : integer := 14; -- output valid latency
    constant ALU_DELAY       : integer := 3;
    constant VALID_OUT_DELAY : integer := 2;

    constant MAX_RADIUS      : integer := 4;
    constant PIPE_DEPTH      : integer := 7;

    constant PADDED_IMG_W    : integer := C_MAX_IMG_W + (2 * MAX_RADIUS);
    constant PADDED_IMG_H    : integer := C_MAX_IMG_H + (2 * MAX_RADIUS);
end package CONSTANTS;

package body CONSTANTS is
end package body CONSTANTS;   

-- states 
package MY_STATES_PKG is 
    type state_t is(Idle, Config, SkipProcessing, Processing);
end package;

package body MY_STATES_PKG is 
end package body;   

