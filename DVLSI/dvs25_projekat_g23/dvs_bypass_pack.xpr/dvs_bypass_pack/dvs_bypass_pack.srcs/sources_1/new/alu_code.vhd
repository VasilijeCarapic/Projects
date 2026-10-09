library IEEE;  
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.RAM_PACKAGE.all;
use work.ARR_TYPES.all; 
use work.MY_STATES_PKG.all;
use work.CONSTANTS.all;

use work.window_pkg.all; 
use work.alu_pkg.all;
use work.alu_window_shift_pkg.all;
use work.compute_win_pkg.all;

entity alu_logic is  
    Port ( reset            : in STD_LOGIC;
           clk              : in STD_LOGIC;
           
           -- input signali 
           img_r            : in  std_logic_vector( 2 downto 0);
           mode             : in std_logic;
           
           -- signal koji prati ulazni pixel kad je validan a kad nije validan  
           reg_input        : in std_logic_vector( 7 downto 0);
           reg_tvalid       : in std_logic;
           reg_tlast        : in std_logic;
           
           -- signali za kontrolu state-a 
           img_processing   : in std_logic;
           skip_mode        : in std_logic;
           config_mode      : in  std_logic;
           processing_done  : out std_logic;
           
           -- stallovanje pipeline-a 
           pipe_en          : in std_logic;
           in_handshake     : in std_logic;
           
           -- sirina i visina slike 
           imgW             : in std_logic_vector(15 downto 0);
           imgH             : in std_logic_vector(15 downto 0);
           shift            : in std_logic_vector( 2 downto 0);
           
           -- koeficijenti u std_logic_vectoru 
           filter_coeffs    : in arr16bit(0 to 80);
           reg_coeff_scale  : in std_logic_vector(15 downto 0);
           
           -- Trenutno stanje koje je  
           state            :  in state_t;
           output_enable    :  out std_logic;
           last_pixel       :  out std_logic;
           pixel_output     :  out std_logic_vector(15 downto 0)
           
           );
end entity alu_logic;

architecture rtl of alu_logic is       

-- ram kontrolni signali 
signal enb   : std_logic;
signal wea   : std_logic;
signal addra : std_logic_vector((clogb2(C_MAX_IMG_W)-1) downto 0) ;
signal addrb : std_logic_vector((clogb2(C_MAX_IMG_W)-1) downto 0) ;

-- ono sto upisujemo u ram 
signal ram_reg_wr: std_logic_vector(63 downto 0) := (others => '0');

-- ono sto citamo iz rama 
signal ram_reg_rd: std_logic_vector(63 downto 0) := (others => '0');

-- signal koji kaze kada nam je prvi validan output
signal ram_ctr_std: std_logic_vector(23 downto 0);

-- debug u ili 
signal col, row             : std_logic_vector(15 downto 0);
attribute mark_debug        : string;
attribute mark_debug of row : signal is "true";
attribute mark_debug of col : signal is "true";

-- delay registri za citanje  
signal pipe: arr8bit(0 to PIPE_DEPTH - 1) := (others => (others => '0'));

-- 9 x 9 array 
signal alu: arr8bit(0 to 80) := (others => (others => '0'));

-- medjurezultati 
signal out_en_inter, proc_done_inter    : std_logic;
signal pixel_out_inter : std_logic_vector(15 downto 0);
signal last_pixel_inter : std_logic;

-- izlazni W, H
signal W_out, H_out: natural;
signal addra_cnt : integer := 0;

-- kontrolni signali za pixel_row i pixel_col 
signal in_col, in_row      : std_logic_vector(15 downto 0);

-- bonus 2 signali 
signal rd_modulo, wr_modulo, data_modulo: std_logic_vector(1 downto 0);

signal start_output : std_logic;
signal clk_en       : std_logic;

begin
    
    clk_en  <= pipe_en;
    
    ADDRESS_GENERATOR: entity work.address_gen(rtl) 
        port map(
            clk => clk, 
            reset => reset, 
            
            img_processing => img_processing, 
            skip_mode      => skip_mode, 
            ram_en         => clk_en,   
            
            row            => row, 
            col            => col,
            imgW           => imgW,
            img_r          => img_r, 
            
            enb            => enb, 
            wea            => wea, 
            addra          => addra,
            addrb          => addrb,
            shift          => shift, 
            
            rd_modulo      => rd_modulo, 
            wr_modulo      => wr_modulo,
            data_modulo    => data_modulo
        ); 
        
    WINDOW_ENGINE: entity work.window_engine
    port map(
        clk           => clk, 
        reset         => reset,
    
        state         => state,
        clk_en        => clk_en,  
    
        img_r         => img_r,
        ram_ctr_std   => ram_ctr_std,
    
        pipe          => pipe,
        ram_reg_rd    => ram_reg_rd,
    
        alu_out       => alu,
        ram_reg_wr    => ram_reg_wr,
        
        rd_modulo     => rd_modulo, 
        wr_modulo     => wr_modulo, 
        data_modulo   => data_modulo
    );    
    
    -- instanciranje ram komponente 
    RAM_BUFFER: entity work.simple_dual_port_ram(Behavioral) 
        generic map(
            G_RAM_WIDTH => 64, 
            G_RAM_DEPTH => C_MAX_IMG_W,
            G_RAM_PERFORMANCE => "HIGH_PERFORMANCE"
        )
        port map(
            addra  => addra, 
            addrb  => addrb, 
            dina   => ram_reg_wr,
            clka   => clk, 
            wea    => wea, 
            enb    => enb,
            rstb   => reset,
            regceb => clk_en,
            doutb  => ram_reg_rd
        );
         
    -- prosledim 9 x 9 prozor i poseban modul mi izracuna output i vrati vrednost iz pixela 
    GRID_PROCESSOR: 
    entity work.grid_system(rtl)
        port map( 
            clk             => clk, 
            reset           => reset, 
            
            input_pixel     => reg_input, 
            reg_tvalid      => reg_tvalid, 
            reg_tlast       => reg_tlast,
            
            img_r           => img_r, 
            filter_coeffs   => filter_coeffs,
            reg_coeff_scale => reg_coeff_scale,  
            
            imgW            => imgW,
            imgH            => imgH,  
            mode            => mode,
            
            img_processing  => img_processing, 
            skip_mode       => skip_mode, 
            config_mode     => config_mode, 
            in_handshake    => in_handshake, 
                     
            state           => state,
            ram_ctr         => ram_ctr_std,
            
            alu             => alu,             

            in_row          => row,
            in_col          => col, 
            
            clk_en          => clk_en,
            start_output    => start_output,
            
            output_enable   => out_en_inter,
            processing_done => proc_done_inter,
            last_pixel      => last_pixel_inter,
            output_val      => pixel_out_inter 
        );
        
    COORD_TRACKER: 
    entity work.coordinate_tracker(rtl)
        port map(
            clk   => clk, 
            reset => reset,
            
            skip_mode      => skip_mode, 
            img_processing => img_processing, 
            
            reg_valid      => reg_tvalid    ,
            clk_en         => clk_en       ,
            state          => state        ,
            
            imgW           => imgW , 
            imgH           => imgH , 
            imgR           => img_r, 
            
            in_col         => col , 
            in_row         => row , 
                
            std_ram_ctr    => ram_ctr_std, 
            
            out_en_inter   => out_en_inter, 
            proc_done_inter=> proc_done_inter,
            pixel_out_inter=> pixel_out_inter,
            
            output_enable  => output_enable, 
            processing_done=> processing_done, 
            pixel_output   => pixel_output
        );
    
    -- pipe delay ram_en = 1 
    reg_delay_proc: process(clk)
    begin 
        if rising_edge(clk) then 
            if reset = '1' then
                pipe <= (others => (others => '0'));
            elsif clk_en = '1' then 
                pipe(0) <= reg_input;
                for i in 1 to pipe'high loop
                    pipe(i) <= pipe(i-1);
                end loop;
            end if;
        end if;
    end process reg_delay_proc;

    -- Poravnavanje last_pixel signala (da ima isto 1-taktno kasnjenje sa clk_en kao i ostali izlazi)
    last_pixel_align_proc: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                last_pixel <= '0';
            elsif clk_en = '1' then
                last_pixel <= last_pixel_inter;
            end if;
        end if;
    end process last_pixel_align_proc;

end architecture rtl;