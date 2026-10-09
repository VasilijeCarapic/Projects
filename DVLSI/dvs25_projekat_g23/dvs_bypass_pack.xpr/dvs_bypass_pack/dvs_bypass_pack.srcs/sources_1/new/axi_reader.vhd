library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.MY_STATES_PKG.ALL;

entity filter_fsm is
    generic(
        C_S_AXI_ADDR_WIDTH : integer := 10;
        ADDR_LSB           : natural := 2
    );
    port (
        clk      : in  std_logic;
        reset    : in  std_logic;
         
        bypass_in  : in  std_logic;
        border_in  : in  std_logic_vector(1 downto 0);
        
        s_axis_tready : in std_logic;
        s_axis_tvalid : in std_logic;
        regs_written  : in std_logic;
        
        processing_done    : in std_logic;
        img_processing     : out std_logic;
        skip_mode          : out std_logic;
        config_mode        : out std_logic;
        
        lock_config        : out std_logic;
        state              : out state_t;
        next_state         : out state_t
    );
end entity filter_fsm;

architecture rtl of filter_fsm is
    signal state_tmp, next_state_tmp : state_t;
    signal start_active : std_logic := '0'; -- desio se prvi handshake 
    
    -- latch processing_done da ne izgubimo puls dok je FSM u Processing/SkipProcessing
    signal proc_done_latch : std_logic := '0'; -- proc_done = '1' jedan takt kasnije 
begin 
    -- FSM registar
    fsm_reg: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                state_tmp <= Idle;
            else
                state_tmp <= next_state_tmp;
            end if;
        end if;
    end process;
    
    -- latch processing_done - setuje se kad stigne, brise se kad se vratimo u Config
    proc_done_latch_proc: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                proc_done_latch <= '0';
            elsif processing_done = '1' then
                proc_done_latch <= '1';
            elsif state_tmp = Config then
                -- resetuj kad smo se vratili u Config
                proc_done_latch <= '0';
            end if;
        end if;
    end process;
    
    -- Next state logika
    next_state_logic: process(state_tmp, bypass_in,
                          s_axis_tready, s_axis_tvalid,
                          processing_done, proc_done_latch, reset)
    begin
        next_state_tmp <= state_tmp;
    
        case state_tmp is
            
            when Idle   => 
                if reset = '1' then 
                    next_state_tmp <= Idle;
                else 
                    next_state_tmp <= Config; 
                end if; 
            when Config =>
                if bypass_in = '1' then
                    if (s_axis_tready = '1' and s_axis_tvalid = '1') then
                        next_state_tmp <= SkipProcessing;
                    end if;
                else
                    if (s_axis_tready = '1' and s_axis_tvalid = '1') then
                        next_state_tmp <= Processing;
                    end if;
                end if;
    
            when SkipProcessing =>
                if processing_done = '1' or proc_done_latch = '1' then
                    next_state_tmp <= Idle;  -- jedan takt drain, pa nazad u Config
                end if;
    
            when Processing =>
                if processing_done = '1' or proc_done_latch = '1' then
                    next_state_tmp <= Idle;  -- jedan takt drain, pa nazad u Config
                end if;
    
            when others =>
                next_state_tmp <= Idle;
    
        end case;
    end process;
    

    state <= state_tmp;

    next_state_process: process(clk) is
    begin
        if rising_edge(clk) then
            next_state <= next_state_tmp;
        end if;
    end process next_state_process;
    
    start_active <= '1' when s_axis_tready = '1' and s_axis_tvalid = '1' else '0';
    
    -- kombinaciono izvedena logika da nema kasnjenja za state_tmp / state
    img_processing <= '1' when (state_tmp = Processing or (state_tmp = Config and start_active = '1' and bypass_in = '0')) else '0';
    skip_mode      <= '1' when (state_tmp = SkipProcessing or (state_tmp = Config and start_active = '1' and bypass_in = '1')) else '0';
    config_mode    <= '1' when (state_tmp = Config and start_active = '0') else '0';
    lock_config    <= '1' when (state_tmp = Processing or state_tmp = SkipProcessing) else '0';
    
    
end architecture rtl;