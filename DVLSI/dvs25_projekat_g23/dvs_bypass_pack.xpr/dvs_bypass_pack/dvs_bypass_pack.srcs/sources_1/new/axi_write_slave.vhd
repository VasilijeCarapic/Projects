library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


-- AXI4-Lite Write FSM
entity axi_write_slave is
    Generic (
        C_S_AXI_DATA_WIDTH : integer := 32;
        C_S_AXI_ADDR_WIDTH : integer := 10;
        ADDR_LSB           : natural := 2
    );
    Port (
        clk   : in std_logic;
        reset : in std_logic;

        -- AXI Write Address Channel
        s_axi_awaddr  : in  std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_awvalid : in  std_logic;
        s_axi_awprot  : in  std_logic_vector(2 downto 0);
        s_axi_awready : out std_logic;

        -- AXI Write Data Channel
        s_axi_wdata  : in  std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_wstrb  : in  std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
        s_axi_wvalid : in  std_logic;
        s_axi_wready : out std_logic;

        -- AXI Write Response Channel
        s_axi_bresp  : out std_logic_vector(1 downto 0);
        s_axi_bvalid : out std_logic;
        s_axi_bready : in  std_logic;

        -- Output: decoded write address and write strobe
        reg_waddr       : out std_logic_vector(C_S_AXI_ADDR_WIDTH-ADDR_LSB-1 downto 0);
        axi_write_ready : out std_logic
    );
end entity axi_write_slave;

architecture rtl of axi_write_slave is

    signal axi_awready : std_logic;
    signal axi_wready  : std_logic;
    signal axi_bvalid  : std_logic;
    signal axi_awaddr  : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);

    type fsm_write_state_type is (WriteAddress, WriteData, WriteStalled);
    signal fsm_axi_write_state : fsm_write_state_type;

    signal write_ready_i : std_logic;

begin

    s_axi_bresp <= "00";

    write_fsm: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                axi_awready <= '0';
                axi_wready  <= '0';
                axi_bvalid  <= '0';
                fsm_axi_write_state <= WriteAddress;
            else
                case fsm_axi_write_state is

                    when WriteAddress =>
                        axi_awready <= '1';
                        axi_wready  <= '1';
                        if axi_awready = '1' and s_axi_awvalid = '1' then
                            axi_awaddr <= s_axi_awaddr;
                            if axi_wready = '1' and s_axi_wvalid = '1' then
                                axi_bvalid <= '1';
                                if s_axi_bready = '0' then
                                    axi_awready <= '0';
                                    axi_wready  <= '0';
                                    fsm_axi_write_state <= WriteStalled;
                                end if;
                            else
                                axi_awready <= '0';
                                fsm_axi_write_state <= WriteData;
                                if s_axi_bready = '1' and axi_bvalid = '1' then
                                    axi_bvalid <= '0';
                                end if;
                            end if;
                        else
                            if s_axi_bready = '1' and axi_bvalid = '1' then
                                axi_bvalid <= '0';
                            end if;
                        end if;

                    when WriteData =>
                        if axi_wready = '1' and s_axi_wvalid = '1' then
                            axi_bvalid <= '1';
                            if s_axi_bready = '0' then
                                axi_awready <= '0';
                                axi_wready  <= '0';
                                fsm_axi_write_state <= WriteStalled;
                            else
                                axi_awready <= '1';
                                axi_wready  <= '1';
                                fsm_axi_write_state <= WriteAddress;
                            end if;
                        else
                            if s_axi_bready = '1' and axi_bvalid = '1' then
                                axi_bvalid <= '0';
                            end if;
                        end if;

                    when WriteStalled =>
                        if s_axi_bready = '1' and axi_bvalid = '1' then
                            axi_bvalid  <= '0';
                            axi_awready <= '1';
                            axi_wready  <= '1';
                            fsm_axi_write_state <= WriteAddress;
                        end if;

                    when others =>
                        axi_awready <= '0';
                        axi_wready  <= '0';
                        axi_bvalid  <= '0';
                        fsm_axi_write_state <= WriteAddress;
                end case;
            end if;
        end if;
    end process write_fsm;

    s_axi_awready <= axi_awready;
    s_axi_wready  <= axi_wready;
    s_axi_bvalid  <= axi_bvalid;

    -- Decoded write address (word-addressed)
    reg_waddr <= s_axi_awaddr(C_S_AXI_ADDR_WIDTH-1 downto ADDR_LSB)
                    when s_axi_awvalid = '1'
                    else axi_awaddr(C_S_AXI_ADDR_WIDTH-1 downto ADDR_LSB);

    -- Write ready pulse: active when address + data both valid
    write_ready_i <= '1' when
        (fsm_axi_write_state = WriteAddress and s_axi_awvalid = '1' and s_axi_wvalid = '1') or
        (fsm_axi_write_state = WriteData    and s_axi_wvalid = '1')
        else '0';

    axi_write_ready <= write_ready_i;

end architecture rtl;