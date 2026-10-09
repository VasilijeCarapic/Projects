library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


-- AXI4-Lite Read Channel FSM
-- Handles: arready, rvalid, rready handshake
-- Output: reg_raddr for register mux
entity axi_read_slave is
    Generic (
        C_S_AXI_DATA_WIDTH : integer := 32;
        C_S_AXI_ADDR_WIDTH : integer := 10;
        ADDR_LSB           : natural := 2
    );
    Port (
        clk   : in std_logic;
        reset : in std_logic;

        -- AXI Read Address Channel
        s_axi_araddr  : in  std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_arprot  : in  std_logic_vector(2 downto 0);
        s_axi_arvalid : in  std_logic;
        s_axi_arready : out std_logic;

        -- AXI Read Data Channel
        s_axi_rdata  : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_rvalid : out std_logic;
        s_axi_rready : in  std_logic;
        s_axi_rresp  : out std_logic_vector(1 downto 0);

        -- Decoded read address for register file mux
        reg_raddr : out std_logic_vector(C_S_AXI_ADDR_WIDTH-ADDR_LSB-1 downto 0);

        -- Read data from register file
        reg_rdata : in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0)
    );
end entity axi_read_slave;

architecture rtl of axi_read_slave is

    signal axi_arready : std_logic;
    signal axi_rvalid  : std_logic;
    signal axi_araddr  : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);

    type fsm_read_state_type is (ReadAddress, ReadData);
    signal fsm_axi_read_state : fsm_read_state_type;

begin

    s_axi_rresp <= "00";

    read_fsm: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                axi_arready <= '0';
                axi_rvalid  <= '0';
                fsm_axi_read_state <= ReadAddress;
            else
                case fsm_axi_read_state is
                    when ReadAddress =>
                        axi_arready <= '1';
                        if axi_arready = '1' and s_axi_arvalid = '1' then
                            axi_araddr  <= s_axi_araddr;
                            axi_arready <= '0';
                            axi_rvalid  <= '1';
                            fsm_axi_read_state <= ReadData;
                        end if;
                    when ReadData =>
                        if s_axi_rready = '1' and axi_rvalid = '1' then
                            axi_rvalid  <= '0';
                            axi_arready <= '1';
                            fsm_axi_read_state <= ReadAddress;
                        end if;
                end case;
            end if;
        end if;
    end process read_fsm;

    s_axi_arready <= axi_arready;
    s_axi_rvalid  <= axi_rvalid;
    s_axi_rdata   <= reg_rdata;

    reg_raddr <= s_axi_araddr(C_S_AXI_ADDR_WIDTH-1 downto ADDR_LSB)
                    when s_axi_arvalid = '1'
                    else axi_araddr(C_S_AXI_ADDR_WIDTH-1 downto ADDR_LSB);

end architecture rtl;