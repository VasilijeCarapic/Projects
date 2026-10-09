library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.CONSTANTS.ALL;
use work.MY_STATES_PKG.all;

-- m_tdata: Ulaz -> m_axis_tdata -> Izlaz 
entity axi_stream_io_block is
    Port (
        clk               : in  std_logic;
        reset             : in  std_logic;

        pipe_en           : out std_logic;                      
        reg_input         : out std_logic_vector(7 downto 0);
        reg_tvalid        : out std_logic;
        reg_tlast         : out std_logic;

        in_handshake      : out std_logic;
        
        state             : in state_t; 
        skip_mode         : in std_logic; 
        img_processing    : in std_logic; 
        config_mode       : in std_logic;
        
        m_tdata           : in  std_logic_vector(15 downto 0);  
        m_tvalid          : in  std_logic;  
        m_tlast           : in  std_logic;  

        s_axis_tdata      : in  std_logic_vector(7 downto 0);
        s_axis_tvalid     : in  std_logic;
        s_axis_tready     : out std_logic;
        s_axis_tlast      : in  std_logic;

        m_axis_tdata      : out std_logic_vector(15 downto 0);
        m_axis_tvalid     : out std_logic;
        m_axis_tready     : in  std_logic;
        m_axis_tlast      : out std_logic
    );
end axi_stream_io_block;

architecture rtl of axi_stream_io_block is

    constant PIPELINE_DEPTH : natural := 1;

    signal tvalid_fifo : std_logic_vector(0 to PIPELINE_DEPTH-1) := (others => '0');
    signal tlast_fifo  : std_logic_vector(0 to PIPELINE_DEPTH-1) := (others => '0');

    signal reg_data     : std_logic_vector(7 downto 0) := (others => '0');

    signal buff_tdata   : std_logic_vector(7 downto 0) := (others => '0');
    signal buff_tvalid  : std_logic := '0';
    signal buff_tlast   : std_logic := '0';
    signal buff_flag    : std_logic := '0';

    signal s_axis_tready_i : std_logic := '0';

begin

    s_axis_tready <= s_axis_tready_i;

    process (clk) is
    begin
        if (rising_edge(clk)) then
            if (reset = '1') then
                tvalid_fifo <= (others => '0');
                tlast_fifo  <= (others => '0');
                reg_data    <= (others => '0');
                buff_tdata  <= (others => '0');
                buff_tvalid <= '0';
                buff_tlast  <= '0';
                buff_flag   <= '0';
            else
                if (m_tvalid = '0' or m_axis_tready = '1') then
                    if (buff_flag = '1') then
                        reg_data       <= buff_tdata;
                        tvalid_fifo(0) <= buff_tvalid;
                        tlast_fifo(0)  <= buff_tlast;
                        buff_flag      <= '0';
                    else
                        reg_data       <= s_axis_tdata;
                        tvalid_fifo(0) <= s_axis_tvalid;
                        tlast_fifo(0)  <= s_axis_tlast;
                    end if;
                else
                    if (buff_flag = '0') then
                        buff_tdata  <= s_axis_tdata;
                        buff_tvalid <= s_axis_tvalid;
                        buff_tlast  <= s_axis_tlast;
                        buff_flag   <= '1';
                    end if;
                end if;
            end if;
        end if;
    end process;

--    process (clk) is
--    begin
--        if (rising_edge(clk)) then
--            if (reset = '1') then
--                s_axis_tready_i <= '0';
--            else
--                if (m_tvalid = '0' or m_axis_tready = '1') then
--                    s_axis_tready_i <= '1';
--                else
--                    s_axis_tready_i <= '0';
--                end if;
--            end if;
--        end if;
--    end process;

    -- proces s_axis_tready_i kombinaciono: 
    s_axis_tready_i <= '1' when (m_tvalid='0' or m_axis_tready='1') else '0'; 
    pipe_en         <= s_axis_tready_i;
    
    -- izlazni signali koji prate ulaz => ova 3 su sinhronizovana  
    reg_input  <= reg_data;
    reg_tvalid <= tvalid_fifo(0);
    reg_tlast  <= tlast_fifo(0);

    in_handshake <= '1' when (s_axis_tvalid = '1' and s_axis_tready_i = '1' 
                          and (img_processing = '1' or skip_mode = '1')) else '0';
                          
    m_axis_tdata  <= m_tdata;
    m_axis_tvalid <= m_tvalid;
    m_axis_tlast  <= m_tlast;
    
end architecture rtl;