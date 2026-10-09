library IEEE;  
USE IEEE.std_logic_1164.all;    
USE IEEE.numeric_std.all;  
USE STD.TEXTIO.all;  

entity tb_fir_filtar is  
end tb_fir_filtar; 

architecture behavioral of tb_fir_filtar is  
    component fir_filtar is  
        generic(
            fifo_counter_max: natural := 256;
            line_length: natural := 256;
            last_writing_counter_address: natural := 259
        );  
        port(  
            clk     : in    std_logic;              
            reset   : in    std_logic;                
            output: out std_logic_vector(7 downto 0)
        );
    end component;  
    signal clk  : std_logic := '0';  
    signal reset : std_logic := '0';                                    
    signal Dout : std_logic_vector(7 downto 0) := "11111111";   
   


begin  
    FIR_inst : FIR_filtar
        generic map (  
            fifo_counter_max => 256,
            line_length => 256,
            last_writing_counter_address => 259
        )  
        port map (  
          
            clk => clk,  
            reset => reset, 
            output => Dout
        );   

    clk <= not clk after 2.5 ns;   
     
   

end behavioral;