library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.RAM_PACKAGE.all;
use work.ARR_TYPES.all;
use work.MY_STATES_PKG.all;
use work.CONSTANTS.all;

entity convolution_core is
    Port (
        clk : in std_logic;
        reset : in std_logic;

        img_processing : in std_logic;
        pipe_en        : in std_logic;   -- freeze pipeline kad backpressure
        mode           : in std_logic;

        state: in state_t;

        alu             : in arr8bit(0 to 80);
        filter_coeffs   : in arr16bit(0 to 80);
        reg_coeff_scale : in std_logic_vector(15 downto 0);

        acc_out : out std_logic_vector(15 downto 0)
    );
end convolution_core;

architecture Behavioral of convolution_core is

    -- sirina jednog proizvoda: pixel se sirenjem predznaka prosiruje na 9 bita
    -- (unsigned 8-bit -> signed 9-bit, uvek pozitivan), koeficijent je signed 16-bit
    -- 9 + 16 = 25 bita
    constant PROD_W : natural := 25;

    type prod_arr_t is array(0 to 80) of signed(PROD_W-1 downto 0);

    -- Stage 1 registar (JEDAN, ne dva) - popunjava se direktno u clk procesu
    -- unutar generate bloka (svaka instanca = svoj mnozac + svoj registar)
    signal products_reg : prod_arr_t;

    -- ROW: 9 partial-sums, svaka sabira 9 clanova sirine PROD_W
    -- potreban rast: ceil(log2(9)) = 4 bita
    constant ROW_W : natural := PROD_W + 4;  -- 29

    type mul_reg_arr is array(0 to 8) of signed(ROW_W-1 downto 0);
    signal mul_reg : mul_reg_arr;

    -- TREE sirine (svaki sledeci nivo sabira 2 clana -> +1 bit)
    constant L1_W : natural := ROW_W + 1; -- 30
    constant L2_W : natural := L1_W + 1;  -- 31

    signal sum01, sum23, sum45, sum67 : signed(L1_W-1 downto 0);
    signal sum89 : signed(ROW_W-1 downto 0);

    signal sum_lvl2_0, sum_lvl2_1, sum_lvl2_2 : signed(L2_W-1 downto 0);

    constant ACC_WIDTH : natural := 34;
    signal final_sum : signed(ACC_WIDTH-1 downto 0);
    signal sum_before_scale : signed(ACC_WIDTH-1 downto 0);

    signal scaled_sum: signed(50 downto 0);
    signal acc : signed(50 downto 0);

begin

    mult_proc: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                products_reg <= (others => (others => '0'));
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' then
                            for i in 0 to 80 loop
                                products_reg(i) <= signed('0' & alu(i)) * signed(filter_coeffs(i));
                            end loop;
                        end if;
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process mult_proc;
    --------------------------------------------------------------------
    -- Stage 2: zbir redova (9 clanova po redu -> 9 partial sums)
    --------------------------------------------------------------------
    process(clk)
        variable tmp : mul_reg_arr;
        variable acc_row : signed(ROW_W-1 downto 0);
    begin
        if rising_edge(clk) then
            if reset = '1' then
                mul_reg <= (others => (others => '0'));
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' then
                            for r in 0 to 8 loop
                                acc_row := (others => '0');
                                for c in 0 to 8 loop
                                    acc_row := acc_row + resize(products_reg(r*9 + c), ROW_W);
                                end loop;
                                tmp(r) := acc_row;
                            end loop;
                            mul_reg <= tmp;
                        end if;
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process;

    --------------------------------------------------------------------
    -- Stage 3: tree level 1 (4 para sabiranja + neparan 9. red prolazi)
    --------------------------------------------------------------------
    process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                sum01 <= (others => '0');
                sum23 <= (others => '0');
                sum45 <= (others => '0');
                sum67 <= (others => '0');
                sum89 <= (others => '0');
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' then
                            sum01 <= resize(mul_reg(0), L1_W) + resize(mul_reg(1), L1_W);
                            sum23 <= resize(mul_reg(2), L1_W) + resize(mul_reg(3), L1_W);
                            sum45 <= resize(mul_reg(4), L1_W) + resize(mul_reg(5), L1_W);
                            sum67 <= resize(mul_reg(6), L1_W) + resize(mul_reg(7), L1_W);
                            sum89 <= resize(mul_reg(8), ROW_W);
                        end if;
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process;

    --------------------------------------------------------------------
    -- Stage 4: tree level 2
    --------------------------------------------------------------------
    process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                sum_lvl2_0 <= (others => '0');
                sum_lvl2_1 <= (others => '0');
                sum_lvl2_2 <= (others => '0');
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' then
                            sum_lvl2_0 <= resize(sum01, L2_W) + resize(sum23, L2_W);
                            sum_lvl2_1 <= resize(sum45, L2_W) + resize(sum67, L2_W);
                            sum_lvl2_2 <= resize(sum89, L2_W);
                        end if;
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process;

    --------------------------------------------------------------------
    -- Stage 5: finalni zbir (3 clana)
    --------------------------------------------------------------------
    process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                final_sum <= (others => '0');
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' then
                            final_sum <= resize(sum_lvl2_0, ACC_WIDTH) +
                                         resize(sum_lvl2_1, ACC_WIDTH) +
                                         resize(sum_lvl2_2, ACC_WIDTH);
                        end if;
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process;

    --------------------------------------------------------------------
    -- Stage 6: pipeline registar pre skaliranja
    --------------------------------------------------------------------
    process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                sum_before_scale <= (others => '0');
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' then
                            sum_before_scale <= final_sum;
                        end if;
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process;

    --------------------------------------------------------------------
    -- Stage 7: mnozenje sa scale 
    --------------------------------------------------------------------
    process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                scaled_sum <= (others => '0');
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' and img_processing = '1' then   
                            scaled_sum <= sum_before_scale * signed('0' & reg_coeff_scale);
                           
                        end if;
                    when others =>
                        null;
                end case;
            end if;
            -- pipe_en='0': acc stoji, ne gubi vrednost
        end if;
    end process;

    --------------------------------------------------------------------
    -- Stage 8: shift u odnosu na mode  
    --------------------------------------------------------------------
    process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                acc <= (others => '0');
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' and img_processing = '1' then
                            if mode = '0' then
                                acc <= shift_right(scaled_sum, 27);
                            else
                                acc <= shift_right(scaled_sum, 20);
                            end if;
                        end if;
                    when others =>
                        null;
                end case;
            end if;
            -- pipe_en='0': acc stoji, ne gubi vrednost
        end if;
    end process;
    
    --------------------------------------------------------------------
    -- Stage 9: output clamp i format
    --------------------------------------------------------------------
    process(clk)
        variable tmp : std_logic_vector(15 downto 0);
    begin
        if rising_edge(clk) then
            if reset = '1' then
                acc_out <= (others => '0');
            else
                case state is
                    when Processing =>
                        if pipe_en = '1' then
                            if mode = '0' then
                                if acc > to_signed(255, acc'length) then
                                    tmp := x"00FF";
                                elsif acc < 0 then
                                    tmp := x"0000";
                                else
                                    tmp := x"00" & std_logic_vector(acc(7 downto 0));
                                end if;
                            else
                                tmp := std_logic_vector(acc(15 downto 0));
                            end if;

                            acc_out <= tmp;
                        end if;
                    when others =>
                        null;
                end case;
            end if;
            -- pipe_en='0': acc_out stoji
        end if;
    end process;

end Behavioral; 