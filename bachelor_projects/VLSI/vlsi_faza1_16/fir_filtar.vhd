library IEEE;  
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.RAM_definitions_PK.all;  
entity fir_filtar is 
generic(
    fifo_counter_max: natural := 256;
    line_length: natural := 256;
    last_writing_counter_address: natural:= 259
    ); 
Port( 
    clk: in std_logic;
    reset: in std_logic;
    output: out std_logic_vector(7 downto 0)
); 
end fir_filtar;
 


--200Mhz maximum frequency of the full system 5.0ns period

--tested with signal_to_img script

--merge sorter is around 284.5 Mhz maximum frequency
architecture Behavioral of fir_filtar is  

--procedures which help to keep code cleaner and more readable 
procedure fifo_buffering(
    signal fifo_counter: in natural range 0 to fifo_counter_max + 3; 
    signal mem_out: in std_logic_vector(7 downto 0);
    signal fifo_in: out std_logic_vector(7 downto 0);
    signal reg_1: out std_logic_vector(7 downto 0);
    signal reg_2: out std_logic_vector(7 downto 0);
    signal reg_3: out std_logic_vector(7 downto 0)
 ) is 
begin
    -- Memory output handling for FIFOS
    case fifo_counter is
        when 0 to 2 =>  
            null;
        when 3 =>   
            reg_1 <= mem_out;    
        when 4=>    
            reg_2 <= mem_out;    
        when 5=>    
            reg_3 <= mem_out;    
        when others =>  
            fifo_in <= mem_out;
    end case;
        
end procedure fifo_buffering;


procedure fifo_logic(
    signal fifo_counter : in natural range 0 to fifo_counter_max + 3;
    signal row : in natural;
    enable_condition : in boolean;
    signal addrb : out std_logic_vector(15 downto 0);
    signal mem_out : in std_logic_vector(7 downto 0);
    signal fifo_in : out std_logic_vector(7 downto 0);
    signal reg_1 : out std_logic_vector(7 downto 0);
    signal reg_2 : out std_logic_vector(7 downto 0);
    signal reg_3 : out std_logic_vector(7 downto 0)
) is
begin
    if (fifo_counter < fifo_counter_max + 3 and enable_condition) then
        if (fifo_counter < fifo_counter_max) then
            addrb <= std_logic_vector(to_unsigned(256 * row + fifo_counter, addrb'length));
        end if;

        fifo_buffering(fifo_counter, mem_out, fifo_in, reg_1, reg_2, reg_3);
    end if;
end procedure;


procedure assign_last3_values(
    signal cnt : in natural range 0 to 5;
    signal mem_out: in std_logic_vector(7 downto 0);
    signal reg_1: out std_logic_vector(7 downto 0); 
    signal reg_2: out std_logic_vector(7 downto 0);
    signal reg_3: out std_logic_vector(7 downto 0)
) is 
begin
    case cnt is
        when 3 =>
            reg_1 <= mem_out;
        when 4=>
            reg_2 <= mem_out;
        when 5=>
            reg_3 <= mem_out;
        when others =>  
            null;
    end case; 
end procedure assign_last3_values;

procedure reset_signals (
    signal last3_memory_addresses_counter : out natural range 0 to 5;
    signal first_writing_in_memory : out boolean;
    signal first_mediana_counter : out natural range 0 to 5;
    signal fifos_buffered_initially: out boolean;
    signal prev_clk_addra: out std_logic_vector(15 downto 0);
    signal dina: out std_logic_vector(7 downto 0);
    signal addrb: out std_logic_vector(15 downto 0);
    signal wea: out std_logic;
    signal din_1: out std_logic_vector(7 downto 0);
    signal din_2: out std_logic_vector(7 downto 0) 
) is
begin
     last3_memory_addresses_counter <= 0; first_writing_in_memory <= false;
     
     first_mediana_counter <= 0;  fifos_buffered_initially <= false;
     
     prev_clk_addra <= std_logic_vector(to_unsigned(257, prev_clk_addra'length));
     
     dina <= (others => '0');  addrb <= (others=>'0');   wea <= '0';
     
     din_1 <= (others => '0');  din_2 <= (others => '0');    
end procedure reset_signals;



procedure wait_for_valid_fifo_read 
(
    signal first_writing_in_memory: out boolean;
    signal first_mediana_counter: inout natural range 0 to 5
)
is 
begin
    if(first_mediana_counter < 6) then
        case first_mediana_counter is
            when 3 =>   
                first_writing_in_memory <= true; 
                first_mediana_counter <= 0;  
            when others=>
                first_mediana_counter <= first_mediana_counter + 1;
        end case;
    end if; 
end procedure wait_for_valid_fifo_read;


--memory instancing
constant C_RAM_WIDTH : integer := 8;             		    -- Specify RAM data width
constant C_RAM_DEPTH : integer := 256*256; 				        -- Specify RAM depth (number of entries)
constant C_RAM_PERFORMANCE : string := "HIGH_PERFORMANCE";
signal addra : std_logic_vector((clogb2(C_RAM_DEPTH)-1) downto 0) := "0000000100000000";     -- Write address bus, width determined from RAM_DEPTH
signal addrb : std_logic_vector((clogb2(C_RAM_DEPTH)-1) downto 0) := (others => '0');     -- Read address bus, width determined from RAM_DEPTH
signal dina  : std_logic_vector(C_RAM_WIDTH-1 downto 0);		  -- RAM input data                    			  -- Clock
signal wea   : std_logic := '0';                     			  -- Write enable                      			  -- RAM Enable, for additional power savings, disable port when not in use                      			  -- Output reset (does not affect memory contents)                    			  -- Output register enable
signal memory_output : std_logic_vector(C_RAM_WIDTH-1 downto 0); 

--signals for first fifo
signal fifo_wr_1: std_logic := '0';
signal fifo_rd_1: std_logic := '0';
signal din_1: std_logic_vector(7 downto 0);
signal output_signal_1: std_logic_vector(7 downto 0) := (others => '0');
 
--signals for second fifo 
signal fifo_wr_2: std_logic := '0';
signal fifo_rd_2: std_logic := '0';
signal din_2: std_logic_vector(7 downto 0);
signal output_signal_2: std_logic_vector(7 downto 0);
--odd_even_merge instancing
signal mediana: std_logic_vector(7 downto 0) := (others => '0');
signal all_read_elements: std_logic_vector(71 downto 0) := (others=>'0');

--declaring merge_sorter 
component odd_even_merge is
    Generic(
        number_of_elements: natural := 8;
        number_of_bits_per_element: natural := 7;
        vector_length: natural := 71
    );
    Port(
        clk: in std_logic;
        reset: in std_logic;
        all_bits_elements: in std_logic_vector(vector_length downto 0);
        s4: out std_logic_vector(number_of_bits_per_element downto 0)
    );  
end component odd_even_merge;

--declaring ram 
component im_ram is
    generic (
        G_RAM_WIDTH : integer := 8;            		    -- Specify RAM data width
        G_RAM_DEPTH : integer := 256*256; 				        -- Specify RAM depth (number of entries)
        G_RAM_PERFORMANCE : string := "HIGH PERFORMANCE"   -- Select "HIGH_PERFORMANCE" or "LOW_LATENCY" 
    );
    port (
        addra : in std_logic_vector((clogb2(G_RAM_DEPTH)-1) downto 0);     -- Write address bus, width determined from RAM_DEPTH
        addrb : in std_logic_vector((clogb2(G_RAM_DEPTH)-1) downto 0);     -- Read address bus, width determined from RAM_DEPTH
        dina  : in std_logic_vector(G_RAM_WIDTH-1 downto 0);		  -- RAM input data
        clka  : in std_logic;                       			  -- Clock
        wea   : in std_logic;                       			  -- Write enable
        enb   : in std_logic;                       			  -- RAM Enable, for additional power savings, disable port when not in use
        rstb  : in std_logic;                       			  -- Output reset (does not affect memory contents)
        regceb: in std_logic;                       			  -- Output register enable
        output_signalb : out std_logic_vector(G_RAM_WIDTH-1 downto 0) 		  -- RAM output data
    );
end component;

--declaring fifo
component ram_fifo is
     generic (
        G_DATAWIDTH : natural := 8;
        G_FIFODEPTH : natural := 253  
    );
    port (
        clk : in std_logic;
        reset : in std_logic;
        fifo_wr : in std_logic;
        din : in std_logic_vector(G_DATAWIDTH-1 downto 0);
        fifo_rd : in std_logic;
        output_signal : out std_logic_vector(G_DATAWIDTH-1 downto 0)
    );
end component;


--internal signals in fir_filtar
signal fifo_counter1: natural range 0 to fifo_counter_max + 3 := 0;
signal fifo_counter2: natural range 0 to fifo_counter_max + 3 := 0;
signal first_mediana_counter: natural range 0 to 5:=0; 
signal fifo1_buffered: boolean := false;
signal fifo2_buffered: boolean := false;
signal first_writing_in_memory: boolean:= false;
signal fifos_buffered_initially: boolean := false;
signal current_row: natural:=1;
signal previous_row: natural:=0;
signal a0,a1,a2,a3,a4,a5,a6,a7,a8: std_logic_vector(7 downto 0):=(others=>'0'); 
signal last3_memory_addresses_counter: natural range 0 to 5 := 0;
signal mem_counter: natural range 514 to 65556 + 2 := 514;
signal permission_flag: natural range 1 to 254 := 1;
signal lower_bound: natural := 256;
signal upper_bound: natural := 511;
signal permission_flag_reg : natural := 0;
signal prev_clk_addra:  std_logic_vector((clogb2(256*256) - 1) downto 0); 

--had to use dont_touch attribute since vivado is optimizing out logic away
attribute dont_touch : string;

attribute dont_touch of addra : signal is "true";
attribute dont_touch of addrb : signal is "true";
attribute dont_touch of dina     : signal is "true";
attribute dont_touch of wea : signal is "true";
attribute dont_touch of memory_output : signal is "true";

attribute dont_touch of fifo_wr_1 : signal is "true";
attribute dont_touch of fifo_rd_1 : signal is "true";
attribute dont_touch of din_1     : signal is "true";
attribute dont_touch of output_signal_1 : signal is "true";

attribute dont_touch of fifo_wr_2 : signal is "true";
attribute dont_touch of fifo_rd_2 : signal is "true";
attribute dont_touch of din_2     : signal is "true";
attribute dont_touch of output_signal_2 : signal is "true";

attribute dont_touch of mediana : signal is "true";
attribute dont_touch of all_read_elements : signal is "true";

attribute dont_touch of fifo_counter1 : signal is "true";
attribute dont_touch of fifo_counter2 : signal is "true";
attribute dont_touch of first_mediana_counter : signal is "true"; 

attribute dont_touch of fifo1_buffered: signal is "true";
attribute dont_touch of fifo2_buffered: signal is "true";
attribute dont_touch of first_writing_in_memory: signal is "true";
attribute dont_touch of fifos_buffered_initially : signal is "true";
attribute dont_touch of current_row: signal is "true";
attribute dont_touch of previous_row: signal is "true";
attribute dont_touch of a0,a1,a2,a3,a4,a5,a6,a7,a8: signal is "true";
attribute dont_touch of last3_memory_addresses_counter: signal is "true";
attribute dont_touch of mem_counter: signal is "true";
attribute dont_touch of permission_flag: signal is "true";
attribute dont_touch of lower_bound: signal is "true";
attribute dont_touch of upper_bound: signal is "true";
attribute dont_touch of permission_flag_reg: signal is "true";
attribute dont_touch of prev_clk_addra: signal is "true"; 

--declare finite state machine
type states is(idle,storing,computing);
signal state_t,nxt_state: states;

begin
odd_even_merge_dut: entity work.odd_even_merge(Behavioral)
    generic map(
        number_of_elements=>8,
        number_of_bits_per_element=> 7,
        vector_length=>71
    )
    port map(
        clk=>clk,
        reset=>reset,
        all_bits_elements=>all_read_elements,
        s4=>mediana
    );
--instancing memory 
dut_memory: entity work.im_ram 
    generic map(
        G_RAM_WIDTH =>C_RAM_WIDTH,           		  
        G_RAM_DEPTH => C_RAM_DEPTH, 				       
        G_RAM_PERFORMANCE => C_RAM_PERFORMANCE  
    )
    port map( 
        addra=>addra, 
        addrb=>addrb, 
        dina =>dina, 
        clka => clk,
        wea => wea,  
        enb => '1', 
        rstb => reset,
        regceb => '1',  
        doutb => memory_output
        );
        
-- first fifo instancing 
fifo_inst_1: entity work.ram_fifo
    generic map (
        G_DATAWIDTH => 8,         
        G_FIFODEPTH => 256         
    )
    port map (
        clk => clk,               
        reset => reset,            
        fifo_wr => fifo_wr_1,      
        din => din_1,              
        fifo_rd => fifo_rd_1,      
        dout => output_signal_1            
    );

-- second fifo instancing 
fifo_inst_2: entity work.ram_fifo
    generic map (
        G_DATAWIDTH => 8,         
        G_FIFODEPTH => 256         
    )
    port map (
        clk => clk,              
        reset => reset,          
        fifo_wr => fifo_wr_2,     
        din => din_2,              
        fifo_rd => fifo_rd_2,    
        dout => output_signal_2            
    );       
   
--logic for fsm    
CURRENT_STATE: process(clk) is 
begin 
    if(rising_edge(clk)) then 
        if(reset='1') then
            state_t <= idle;
        else
            state_t <= nxt_state;
        end if;
    end if; 
end process CURRENT_STATE;   
 
 
 
NEXT_STATE: process(clk) is 
begin 
if(rising_edge(clk)) then
        case state_t is
            when idle=>
                if(fifos_buffered_initially = true) then 
                    nxt_state <= idle;
                else 
                     nxt_state <= storing;
                end if;            
                when storing=>
                   
             if(fifos_buffered_initially = false) then 
                 nxt_state <= storing;
             else 
                 nxt_state <= computing;
             end if;
               
         when computing=>
             if(mem_counter < 65556) then 
                 nxt_state <= computing;
             else 
                 nxt_state <= idle;     
             end if;    
        when others=>
             nxt_state <= idle;   
        end case;  
end if;   
end process NEXT_STATE;   


--main process in which is storing and computing executed
fulfilling_fifos: process(clk) is 
variable previous_addra: natural;  
begin
    if(rising_edge(clk)) then 
        case state_t is 
            when storing=>
                   -- Initial values for rows
                   previous_row <= 0;   current_row <= 1;
  
                   --first fifo logic   
                   fifo_logic(fifo_counter1,previous_row,not fifo1_buffered,addrb,memory_output,
                    din_1,a8,a7,a6);  
                   
                   --second fifo logic
                   fifo_logic(fifo_counter2,current_row,fifo1_buffered and not fifo2_buffered,
                    addrb,memory_output,din_2,a5,a4,a3);
            
                   -- Processing after both FIFOs are buffered
                   if (fifo1_buffered and fifo2_buffered) then
                        
                        if (last3_memory_addresses_counter <= 2) then
                             addrb <= std_logic_vector(to_unsigned(line_length * 2 + last3_memory_addresses_counter, clogb2(C_RAM_DEPTH)));  -- Set address
                        end if;
                       
                       -- Memory output handling for the last memory addresses
                        assign_last3_values(last3_memory_addresses_counter,memory_output,a2,a1,a0);
                        
                        last3_memory_addresses_counter <= last3_memory_addresses_counter + 1;
        
                       -- Set fifos_buffered_initially to true after fullfilling memory addresses
                        if last3_memory_addresses_counter = 5 then
                             fifos_buffered_initially <= true;
                        end if;        
                   end if;     
            when computing=> 
                last3_memory_addresses_counter <= 0; 
                -- Code after both FIFOs are filled, perform final processing
                if(not first_writing_in_memory) then
                          wait_for_valid_fifo_read(first_writing_in_memory,
                            first_mediana_counter);  
                    
                else
                    wea <= '1';  addrb <= std_logic_vector(unsigned(addrb) + 1);
                    if(mem_counter > 516 and mem_counter < 65542) then
                        a0 <= memory_output;
                    end if;
                        
                    if(mem_counter > 520 and mem_counter <  65543 ) then
                        a1 <= a0;
                        a2 <= a1;
                        a3 <= output_signal_2;
                        a4 <= a3;
                        a5 <= a4;
                        a6 <= output_signal_1;
                        a7 <= a6;
                        a8 <= a7;
                            
                        din_2 <= a2;
                        din_1 <= a5;
                        all_read_elements <= a8 & a7 & a6 & a5 & a4 & a3 & a2 & a1 & a0;
                    end if;    
                            
                    if(mem_counter > 533 and mem_counter < 65556 ) then 
                        previous_addra := to_integer(unsigned(addra));
                             
                        if(previous_addra /= lower_bound and previous_addra /= upper_bound) then   
                             prev_clk_addra <= std_logic_vector(unsigned(prev_clk_addra) + 1);
                             dina <= mediana;
                        else 
                             prev_clk_addra <= std_logic_vector(unsigned(prev_clk_addra) + 1); 
                             dina <= (others => '0');    
                        end if;    
                    end if;  
                end if; 
            when idle=>           
                reset_signals(last3_memory_addresses_counter,first_writing_in_memory,
                    first_mediana_counter,fifos_buffered_initially,prev_clk_addra,
                    dina, addrb, wea, din_1, din_2);   
            when others=>
                null;
            end case;    
end if;             
end process fulfilling_fifos;


addra_computing_process: process(clk) is 
begin 
    if(rising_edge(clk)) then 
        case state_t is 
            when computing=>
                addra <= prev_clk_addra;
            when others=>
                addra <= std_logic_vector(to_unsigned(257, addra'length));
        end case;        
    end if;        
end process addra_computing_process;

--process which says when we read,when write in fifos
fifo_enable_signals_process: process(clk) is 
begin 
    if(rising_edge(clk)) then 
        case state_t is 
            when idle=>
                fifo_wr_1 <= '0';  fifo_wr_2 <= '0';
                
                fifo_rd_1 <= '0';  fifo_rd_2 <= '0';
            when storing=>
                if (fifo_counter1 < fifo_counter_max + 3 and not fifo1_buffered) then 
                    fifo_rd_1 <= '0';  fifo_rd_2 <= '0';  fifo_wr_2 <= '0'; 
                    case fifo_counter1 is
                        when 0 to 5 =>
                            null;
                        when others =>
                            fifo_wr_1 <= '1';
                    end case;
                end if;
                
                if (fifo_counter2 < fifo_counter_max + 3 and fifo1_buffered and not fifo2_buffered) then 
                    fifo_wr_1 <= '0';  fifo_rd_2 <= '0';  fifo_rd_1 <= '0';
                    
                    case fifo_counter2 is
                        when 0 to 5 =>
                            null;
                        when others =>
                            fifo_wr_2 <= '1';  
                    end case;
                end if;
                
                
                if (fifo1_buffered and fifo2_buffered) then
                    fifo_wr_1 <= '0'; fifo_rd_1 <= '0';
                    fifo_wr_2 <= '0'; fifo_rd_2 <= '0';
                end if;
                
            when computing=>
                if(not first_writing_in_memory) then
                    fifo_wr_1 <= '0'; fifo_rd_1 <= '0';
                    fifo_wr_2 <= '0'; fifo_rd_2 <= '0'; 
                else    
                    fifo_rd_1 <= '1';  fifo_rd_2 <= '1';                 
                    
                    if(mem_counter > 520) then 
                        fifo_wr_1 <= '1';  fifo_wr_2 <= '1';
                    end if;
                    
                end if;
            when others=>
                null;
        end case;        
    end if;
end process fifo_enable_signals_process;

fifo_counter1_process: process(clk) is 
begin 
    if(rising_edge(clk)) then 
        case state_t is 
            when storing=>
                if (fifo_counter1 < fifo_counter_max + 3 and not fifo1_buffered) then
                    fifo_counter1 <= fifo_counter1 + 1;
            
                    if fifo_counter1 = fifo_counter_max + 2 then
                        fifo1_buffered <= true;
                        fifo_counter1 <= 0;
                    end if;
                end if;
            when idle=> 
                fifo_counter1 <= 0;
                fifo1_buffered <= false;
            when others=> 
                null;
        end case;
    end if;
end process fifo_counter1_process;

fifo_counter2_process: process(clk) is 
begin 
    if(rising_edge(clk)) then 
        case state_t is 
            when storing=>
                if (fifo_counter2 < fifo_counter_max + 3 and fifo1_buffered and not fifo2_buffered) then
                    fifo_counter2 <= fifo_counter2 + 1;
            
                    if fifo_counter2 = fifo_counter_max + 2 then
                        fifo2_buffered <= true;      
                        fifo_counter2 <= 0;
                    end if;
                end if;
            
            when idle=> 
                fifo_counter2 <= 0;
                fifo2_buffered<= false;
            when others=> 
                null;
        end case;
    end if;
end process fifo_counter2_process;

--in which fields should writing mediana value be skipped ? 
perm_flag_counter:process(clk) is
variable previous_addrb: natural range 0 to 65535;
begin 
    if rising_edge(clk) then 
        previous_addrb := to_integer(unsigned(addra));
        case state_t is 
            when computing=>
                if previous_addrb = upper_bound + 1 then 
                                        
                    if(permission_flag < 255) then 
                          permission_flag <= permission_flag + 1 ;
                    else    
                          permission_flag <= 1;
                    end if;
                                                   
                end if;
             when others=>
                permission_flag <= 1;
         end case;   
    end if;            
end process perm_flag_counter;

--find the lower field of the current row in which shouldn ot be written
computing_lower_bound:process(clk)
begin
    if rising_edge(clk) then
        case state_t is 
            when computing =>
                lower_bound <= permission_flag * line_length;
            when others =>
                lower_bound <= 256;
        end case;         
    end if;
end process computing_lower_bound;

--main counter which counts in which row and column(which field are we now) 
mem_counter_process: process(clk) is 
begin 
    if(rising_edge(clk)) then 
        case state_t is 
            when computing =>
                if(mem_counter < 65558) then 
                    mem_counter <= mem_counter+1;
                else 
                    mem_counter <= 514;
                end if;    
            when others =>
                mem_counter <= 514;
        end case;
    end if;
end process mem_counter_process;

--find the upper field of the row in which should not be written
computing_upper_bound:process(clk) is
begin
    if rising_edge(clk) then
    case state_t is 
        when computing=>
            upper_bound <= (permission_flag + 1)*line_length - 1;
        when others=>
            upper_bound <= 511;
    end case;            
    end if;
end process computing_upper_bound;

--logic for output signal 
output_logic: process(clk)
begin
    if(rising_edge(clk)) then 
        case state_t is  
            when computing =>
                output <= mediana;
            
            when others => 
                output <= (others => '0');         
        end case;
    end if;  
end process output_logic;

end architecture behavioral; 