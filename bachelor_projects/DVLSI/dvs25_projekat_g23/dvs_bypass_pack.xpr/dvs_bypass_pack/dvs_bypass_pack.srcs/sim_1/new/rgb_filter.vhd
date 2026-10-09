library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use std.textio.all;
library work;
use work.filter_constants_pkg.all;
use work.filter_config.all;

entity rgb_filter is
end entity;

architecture Behavioral of rgb_filter is
    constant CLK_PERIOD : time := 10 ns;
    constant IMG_WIDTH  : integer := 768;
    constant IMG_HEIGHT : integer := 512;
    constant PLANES     : integer := 3;
    constant PLANE_SIZE : integer := IMG_WIDTH * IMG_HEIGHT;
    constant IMG_SIZE   : integer := PLANE_SIZE * PLANES;

    constant IN_FILE    : string := "parrots.txt";
    constant OUT_FILE   : string := "hw_output.txt";
    
    -- Ovde se bira koji se aktivan filtar testira 
    constant ACTIVE_FILTER : filter_sel_t := SEL_LOG;
    constant ACTIVE_COEFFS : s16_array_81 := get_coeffs(ACTIVE_FILTER);
    
    -- backpressure koliko ce da se pali gasi 
    constant backpressure_val: integer := 4;

    signal reset        : std_logic := '1';
    signal clk          : std_logic := '0';

    -- AXI Lite interfejs
    signal s_axi_ctrl_awaddr  : std_logic_vector(9 downto 0) := (others => '0');
    signal s_axi_ctrl_awprot  : std_logic_vector(2 downto 0) := (others => '0');
    signal s_axi_ctrl_awvalid : std_logic := '0';
    signal s_axi_ctrl_awready : std_logic;
    signal s_axi_ctrl_wdata   : std_logic_vector(31 downto 0) := (others => '0');
    signal s_axi_ctrl_wstrb   : std_logic_vector(3 downto 0) := (others => '0');
    signal s_axi_ctrl_wvalid  : std_logic := '0';
    signal s_axi_ctrl_wready  : std_logic;
    signal s_axi_ctrl_bresp   : std_logic_vector(1 downto 0);
    signal s_axi_ctrl_bvalid  : std_logic;
    signal s_axi_ctrl_bready  : std_logic := '0';
    signal s_axi_ctrl_araddr  : std_logic_vector(9 downto 0) := (others => '0');
    signal s_axi_ctrl_arprot  : std_logic_vector(2 downto 0) := (others => '0');
    signal s_axi_ctrl_arvalid : std_logic := '0';
    signal s_axi_ctrl_arready : std_logic;
    signal s_axi_ctrl_rdata   : std_logic_vector(31 downto 0);
    signal s_axi_ctrl_rvalid  : std_logic;
    signal s_axi_ctrl_rready  : std_logic := '0';
    signal s_axi_ctrl_rresp   : std_logic_vector(1 downto 0);

    -- AXI Stream interfejs
    signal s_axis_data_in_tdata   : std_logic_vector(7 downto 0) := (others => '0');
    signal s_axis_data_in_tvalid  : std_logic := '0';
    signal s_axis_data_in_tready  : std_logic;
    signal s_axis_data_in_tlast   : std_logic := '0';

    signal m_axis_data_out_tdata  : std_logic_vector(15 downto 0);
    signal m_axis_data_out_tvalid : std_logic;
    signal m_axis_data_out_tready : std_logic := '0';
    signal m_axis_data_out_tlast  : std_logic;

    file output_file : TEXT open WRITE_MODE is OUT_FILE;

    type img_type is array (0 to IMG_SIZE - 1) of integer;

    impure function InitImgFromFile (ramfilename : in string) return img_type is
        file imgfile         : text is in ramfilename;
        variable imgfileline : line;
        variable img_name    : img_type;
        variable pixel_int   : integer;
    begin
        for k in img_type'range loop
            if not endfile(imgfile) then
                readline (imgfile, imgfileline);
                read (imgfileline, pixel_int);
                img_name(k) := pixel_int;
            else
                img_name(k) := 0;
            end if;
        end loop;
        return img_name;
    end function;

    signal img_name : img_type := InitImgFromFile(IN_FILE);

begin

    clk_process : process
    begin
        clk <= '0';
        wait for CLK_PERIOD / 2;
        clk <= '1';
        wait for CLK_PERIOD / 2;
    end process;

    DUT: entity work.acc_image_filter
    port map (
        reset => reset,
        clk   => clk,

        s_axi_ctrl_awaddr  => s_axi_ctrl_awaddr,
        s_axi_ctrl_awprot  => s_axi_ctrl_awprot,
        s_axi_ctrl_awvalid => s_axi_ctrl_awvalid,
        s_axi_ctrl_awready => s_axi_ctrl_awready,
        s_axi_ctrl_wdata   => s_axi_ctrl_wdata,
        s_axi_ctrl_wstrb   => s_axi_ctrl_wstrb,
        s_axi_ctrl_wvalid  => s_axi_ctrl_wvalid,
        s_axi_ctrl_wready  => s_axi_ctrl_wready,
        s_axi_ctrl_bresp   => s_axi_ctrl_bresp,
        s_axi_ctrl_bvalid  => s_axi_ctrl_bvalid,
        s_axi_ctrl_bready  => s_axi_ctrl_bready,
        s_axi_ctrl_araddr  => s_axi_ctrl_araddr,
        s_axi_ctrl_arprot  => s_axi_ctrl_arprot,
        s_axi_ctrl_arvalid => s_axi_ctrl_arvalid,
        s_axi_ctrl_arready => s_axi_ctrl_arready,
        s_axi_ctrl_rdata   => s_axi_ctrl_rdata,
        s_axi_ctrl_rvalid  => s_axi_ctrl_rvalid,
        s_axi_ctrl_rready  => s_axi_ctrl_rready,
        s_axi_ctrl_rresp   => s_axi_ctrl_rresp,

        s_axis_data_in_tdata  => s_axis_data_in_tdata,
        s_axis_data_in_tvalid => s_axis_data_in_tvalid,
        s_axis_data_in_tready => s_axis_data_in_tready,
        s_axis_data_in_tlast  => s_axis_data_in_tlast,

        m_axis_data_out_tdata  => m_axis_data_out_tdata,
        m_axis_data_out_tvalid => m_axis_data_out_tvalid,
        m_axis_data_out_tready => m_axis_data_out_tready,
        m_axis_data_out_tlast  => m_axis_data_out_tlast
    );

    stim_proc : process
        variable j, p : integer := 0;

        procedure axi_write(
            constant addr : in std_logic_vector(7 downto 0);
            constant data : in std_logic_vector(31 downto 0)
        ) is
        begin
            wait until rising_edge(clk);
            s_axi_ctrl_awaddr  <= addr & "00";
            s_axi_ctrl_wdata   <= data;
            s_axi_ctrl_awvalid <= '1';
            s_axi_ctrl_wvalid  <= '1';
            s_axi_ctrl_wstrb   <= "0011";
            s_axi_ctrl_bready  <= '1';

            wait until rising_edge(clk) and s_axi_ctrl_awready = '1' and s_axi_ctrl_wready = '1';
            s_axi_ctrl_awvalid <= '0';
            s_axi_ctrl_wvalid  <= '0';

            wait until rising_edge(clk) and s_axi_ctrl_bvalid = '1';
            s_axi_ctrl_bready <= '0';
        end procedure;
    begin
        wait for 4 * CLK_PERIOD;
        reset <= '0';
        wait for CLK_PERIOD;
        wait until rising_edge(clk);

        -- Radius, dimenzije, scale 
        axi_write(x"02", std_logic_vector(to_unsigned(RADIUS_LUT(ACTIVE_FILTER), 32)));
        axi_write(x"04", std_logic_vector(to_unsigned(IMG_WIDTH, 32)));
        axi_write(x"06", std_logic_vector(to_unsigned(IMG_HEIGHT, 32)));
        axi_write(x"08", std_logic_vector(to_unsigned(SCALE_LUT(ACTIVE_FILTER), 32)));

        -- Koeficijenti -- signed vrednosti moraju ici kroz to_signed + resize
        for addr in 0 to 80 loop
            axi_write(
                std_logic_vector(to_unsigned(10 + addr * 2, 8)),
                std_logic_vector(resize(to_signed(ACTIVE_COEFFS(addr), 16), 32))
            );
        end loop;
        
        -- ovde se bira koji mode se izvrsava 
        axi_write(x"00", x"00000001");
        wait for 100 ns;

        for plane_idx in 0 to PLANES - 1 loop
            j := plane_idx * PLANE_SIZE;
            p := 0;

            while p < PLANE_SIZE loop
                s_axis_data_in_tdata  <= std_logic_vector(to_unsigned(img_name(j), 8));
                s_axis_data_in_tvalid <= '1';

                if p = PLANE_SIZE - 1 then
                    s_axis_data_in_tlast <= '1';
                else
                    s_axis_data_in_tlast <= '0';
                end if;

                wait until rising_edge(clk);

                if s_axis_data_in_tvalid = '1' and s_axis_data_in_tready = '1' then
                    j := j + 1;
                    p := p + 1;
                end if;
            end loop;

            s_axis_data_in_tlast  <= '0';
            s_axis_data_in_tvalid <= '0';
            s_axis_data_in_tdata  <= (others => '0');

            -- wait until the design finishes processing the current plane
            wait until rising_edge(clk) and m_axis_data_out_tvalid = '1' and m_axis_data_out_tready = '1' and m_axis_data_out_tlast = '1';
            wait for 10 * CLK_PERIOD;
        end loop;

        wait;
    end process;

    process(clk)
        variable i : integer := 0;
    begin
        if rising_edge(clk) then
            if reset = '1' then
                m_axis_data_out_tready <= '0';
                i := 0;
            else
                if i mod backpressure_val /= 0 then
                    m_axis_data_out_tready <= '0';
                else
                    m_axis_data_out_tready <= '1';
                end if;
                i := i + 1;
            end if;
        end if;
    end process;

    process
        variable output_line : LINE;
    begin
        wait until falling_edge(clk);

        if reset = '0' then
            if m_axis_data_out_tvalid = '1' and m_axis_data_out_tready = '1' then
                write(output_line, to_integer(unsigned(m_axis_data_out_tdata)));
                writeline(output_file, output_line);
            end if;
        end if;
    end process;

end Behavioral;