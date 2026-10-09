library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.ARR_TYPES.all;
use work.CONSTANTS.all;


-- Register File: upis i citanje svih registara akceleratora
-- Ulaz: reg_waddr, axi_write_ready, wstrb, wdata, restrict_reg_input
-- Izlaz: sve vrednosti registara + read data mux
entity register_file is
    Generic (
        C_S_AXI_DATA_WIDTH : integer := 32;
        C_S_AXI_ADDR_WIDTH : integer := 10;
        ADDR_LSB           : natural := 2
    );
    Port (
        clk   : in std_logic;
        reset : in std_logic;

        -- Write interface (iz axi_write_slave)
        reg_waddr       : in std_logic_vector(C_S_AXI_ADDR_WIDTH-ADDR_LSB-1 downto 0);
        axi_write_ready : in std_logic;
        wdata           : in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        wstrb           : in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
        lock_config     : in std_logic;

        -- Read interface (za axi_read_slave)
        reg_raddr : in  std_logic_vector(C_S_AXI_ADDR_WIDTH-ADDR_LSB-1 downto 0);
        rdata     : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);

        -- Register outputs
        reg_ctrl_out        : out std_logic_vector(15 downto 0);
        reg_radius_out      : out std_logic_vector(15 downto 0);
        reg_img_w_out       : out std_logic_vector(15 downto 0);
        reg_img_h_out       : out std_logic_vector(15 downto 0);
        reg_coeff_scale_out : out std_logic_vector(15 downto 0);
        reg_coeffs_out      : out arr16bit(0 to 80);

        -- regs_written: '1' kad je CTRL registar upisan (poslednji u C kodu)
        regs_written        : out std_logic

    );
end entity register_file;

architecture rtl of register_file is

    constant REG_CTRL_ADDR        : std_logic_vector(7 downto 0) := x"00";
    constant REG_RADIUS_ADDR      : std_logic_vector(7 downto 0) := x"02";
    constant REG_IMG_W_ADDR       : std_logic_vector(7 downto 0) := x"04";
    constant REG_IMG_H_ADDR       : std_logic_vector(7 downto 0) := x"06";
    constant REG_COEFF_SCALE_ADDR : std_logic_vector(7 downto 0) := x"08";

    signal reg_ctrl        : std_logic_vector(15 downto 0);
    signal reg_radius      : std_logic_vector(15 downto 0);
    signal reg_img_w       : std_logic_vector(15 downto 0);
    signal reg_img_h       : std_logic_vector(15 downto 0);
    signal reg_coeff_scale : std_logic_vector(15 downto 0);
    signal filter_coeffs   : arr16bit(0 to 80); -- koeficijenti koji se upisuju 

    signal reg_coeff_W   : arr8bit(0 to 80); -- adrese koeficijenata 
    signal regs_written_i : std_logic := '0';  
    
    attribute max_fanout : integer;
    attribute max_fanout of filter_coeffs : signal is 150;

begin

    -- Koeficijentne adrese: 10, 12, 14, ... (stride 2)
    gen_coeff_addr: for i in 0 to 80 generate
        reg_coeff_W(i) <= std_logic_vector(to_unsigned(10 + i*2, 8));
    end generate;


    -- Upis glavnih registara
    write_main_regs: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                reg_ctrl        <= (others => '0');
                reg_radius      <= std_logic_vector(to_unsigned(1,   16));
                reg_img_w       <= std_logic_vector(to_unsigned(128, 16));
                reg_img_h       <= std_logic_vector(to_unsigned(128, 16));
                reg_coeff_scale <= std_logic_vector(to_unsigned(9,   16));
            elsif axi_write_ready = '1' and lock_config = '0' then 
                if wstrb(0) = '1' then
                    case reg_waddr is
                        when REG_CTRL_ADDR        => reg_ctrl(7 downto 0)        <= wdata(7 downto 0);
                        when REG_RADIUS_ADDR      => reg_radius(2 downto 0)      <= wdata(2 downto 0);
                        when REG_IMG_W_ADDR       => reg_img_w(7 downto 0)       <= wdata(7 downto 0);
                        when REG_IMG_H_ADDR       => reg_img_h(7 downto 0)       <= wdata(7 downto 0);
                        when REG_COEFF_SCALE_ADDR => reg_coeff_scale(7 downto 0) <= wdata(7 downto 0);
                        when others => null;
                    end case;
                end if;
                if wstrb(1) = '1' then
                    case reg_waddr is
                        when REG_CTRL_ADDR        => reg_ctrl(11 downto 8)        <= wdata(11 downto 8);
                        when REG_IMG_W_ADDR       => reg_img_w(15 downto 8)       <= wdata(15 downto 8);
                        when REG_IMG_H_ADDR       => reg_img_h(15 downto 8)       <= wdata(15 downto 8);
                        when REG_COEFF_SCALE_ADDR => reg_coeff_scale(15 downto 8) <= wdata(15 downto 8);
                        when others => null;
                    end case;
                end if;
            end if;
        end if;
    end process write_main_regs;


    -- Upis koeficijenata
    gen_coeff_write: for i in 0 to 80 generate
        write_coeff: process(clk)
        begin
            if rising_edge(clk) then
                if reset = '1' then
                    filter_coeffs(i) <= (others => '0');
                elsif axi_write_ready = '1' and lock_config = '0' then
                    if reg_waddr = reg_coeff_W(i) then
                        if wstrb(0) = '1' then
                            filter_coeffs(i)(7 downto 0)  <= wdata(7 downto 0);
                        end if;
                        if wstrb(1) = '1' then
                            filter_coeffs(i)(15 downto 8) <= wdata(15 downto 8);
                        end if;
                    end if;
                end if;
            end if;
        end process write_coeff;
    end generate;

 
    -- Read mux
    read_mux: process(clk)
    begin
        if rising_edge(clk) then
            rdata <= (others => '0');
            case reg_raddr is
                when REG_CTRL_ADDR        => rdata(15 downto 0) <= reg_ctrl;
                when REG_RADIUS_ADDR      => rdata(15 downto 0) <= reg_radius;
                when REG_IMG_W_ADDR       => rdata(15 downto 0) <= reg_img_w;
                when REG_IMG_H_ADDR       => rdata(15 downto 0) <= reg_img_h;
                when REG_COEFF_SCALE_ADDR => rdata(15 downto 0) <= reg_coeff_scale;
                when others => null;
            end case;
            for i in 0 to 80 loop
                if reg_raddr = reg_coeff_W(i) then
                    rdata(15 downto 0) <= filter_coeffs(i);
                end if;
            end loop;
        end if;
    end process read_mux;

    -- regs_written: '1' kad je CTRL registar upisan
    -- Resetuje se kad FSM udje u Config
    regs_written_proc: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                regs_written_i <= '0';
            elsif axi_write_ready = '1' and lock_config = '0' then
                if reg_waddr = REG_CTRL_ADDR then
                    regs_written_i <= '1';
                end if;
            else
                regs_written_i <= '0';
            end if;
        end if;
    end process regs_written_proc;

    regs_written <= regs_written_i;


    -- Izlazni signali su direktno povezani na vec registrovane interne signale   
    reg_ctrl_out        <= reg_ctrl;
    reg_radius_out      <= reg_radius;
    reg_img_w_out       <= reg_img_w;
    reg_img_h_out       <= reg_img_h;
    reg_coeff_scale_out <= reg_coeff_scale;
    reg_coeffs_out      <= filter_coeffs;

end architecture rtl;