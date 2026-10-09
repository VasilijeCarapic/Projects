library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity tb_odd_even_merge is
end tb_odd_even_merge;

architecture behavior of tb_odd_even_merge is
    signal clk                 : std_logic := '0';
    signal reset               : std_logic := '0';
    signal all_bits_elements   : std_logic_vector(71 downto 0):= (others => '0');
    signal s4           : std_logic_vector(7 downto 0):= (others => '0');

    constant number_of_elements      : natural := 8;
    constant number_of_bits_per_element : natural := 7;
    constant vector_length           : natural := 71;
    constant Tclk: time := 3.5ns; 
    begin

    DUT: entity work.odd_even_merge
        generic map (
            number_of_elements => number_of_elements,
            number_of_bits_per_element => number_of_bits_per_element,
            vector_length => vector_length
        )
        port map (
            clk => clk,
            reset => reset,
            all_bits_elements => all_bits_elements,
            s4=>s4
        );

    -- Clock generation
    clk_gen: clk <= not clk after Tclk / 2;
    -- Stimulus process to drive the reset and inputs
    stimulus_process: process
    begin
        reset <= '1';
        wait for 4.5 * Tclk;
        reset <= '0';
        wait for 10*Tclk;

all_bits_elements <= "000000010000001000000011000001000000010100000110000001110000001000110110"; 
wait for 10 * Tclk;

all_bits_elements <= "000000110000001100000011000001010000010100000110000001110000011000010101"; 
wait for 10*Tclk;

all_bits_elements <= "001100110001101100000011000001010000010100000110000001110000011001010001";
wait for 50*Tclk;

all_bits_elements <= "110110010100101111110100100010111101000110101010010110011010110011100010";
wait for 20*Tclk;

all_bits_elements <= "010101010101010101010101010101010101010101010101010101010101010101010101";
wait for 20*Tclk;

all_bits_elements <= "101010101010101010101010101010101010101010101010101010101010101010101010";
wait for 10*Tclk;

all_bits_elements <= "110011001100110011001100110011001100110011001100110011001100110011001100";
wait for 10*Tclk;

all_bits_elements <= "111000011110000111100001111000011110000111100001111000011110000111100001";
wait for 20*Tclk;

all_bits_elements <= "000011110000111100001111000011110000111100001111000011110000111100001111";
wait for 20*Tclk;

all_bits_elements <= "100110011001100110011001100110011001100110011001100110011001100110011001";
wait for 30*Tclk;

all_bits_elements <= "111111000000111111110000000011111111110000000111111111111111111100111111";
wait for 10*Tclk;

all_bits_elements <= "001100110011001100110011001100110011001100110011001100110011001100110011";
wait for 10*Tclk;

all_bits_elements <= "111100110000111111110000000000111100110000111111110000111100110000111100";
wait for 30*Tclk;

all_bits_elements <= "110101101101110111110011011110110101011011011010101110011001101010010110";
wait for 20*Tclk;

all_bits_elements <= "000000110000000000111111111100110011101010010100111111000011100000011000";
wait for 10*Tclk;

all_bits_elements <= "001101110000110011101101100100010011001111010111000010110001000111010011";
wait for 20*Tclk;
wait;
    end process stimulus_process;

end behavior;