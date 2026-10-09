library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity pixel_packer is
    Port (
        clk               : in  std_logic;
        reset             : in  std_logic;
        pipe_en           : in  std_logic;
        config_mode       : in  std_logic;
        
        mode              : in  std_logic;
        img_processing    : in  std_logic;
        skip_mode         : in  std_logic; 
        
        m_tvalid_dly      : in  std_logic;
        m_tlast_dly       : in  std_logic;
        in_px             : in  std_logic_vector(7 downto 0);
        
        packed_data       : out std_logic_vector(15 downto 0);
        packed_valid      : out std_logic;
        packed_last       : out std_logic
    );
end entity pixel_packer;

architecture rtl of pixel_packer is

    type state_t is (WAIT_LOW_PIXEL, WAIT_HIGH_PIXEL);
    signal current_state : state_t := WAIT_LOW_PIXEL;

    signal px_low           : std_logic_vector(7 downto 0)  := (others => '0');
    signal packed_data_reg  : std_logic_vector(15 downto 0) := (others => '0');
    signal packed_valid_reg : std_logic := '0';
    signal packed_last_reg  : std_logic := '0';

begin

    pack_proc: process(clk)
    begin
        if rising_edge(clk) then
            -- Reset na sistemski reset, config_mode ili pri promjeni moda
            if reset = '1' or config_mode = '1' then
                current_state    <= WAIT_LOW_PIXEL;
                px_low           <= (others => '0');
                packed_data_reg  <= (others => '0');
                packed_valid_reg <= '0';
                packed_last_reg  <= '0';
            elsif pipe_en = '1' then
                if (img_processing = '1' and mode = '0') or skip_mode = '1' then
                    case current_state is
                        
                        -- Čekamo prvi (niži) piksel za novu 16-bitnu reč
                        when WAIT_LOW_PIXEL =>
                            if m_tvalid_dly = '1' then
                                px_low <= in_px;
                                if m_tlast_dly = '1' then
                                    -- Neparan broj piksela: poslednji piksel ide sam na niži bajt
                                    packed_data_reg  <= x"00" & in_px;
                                    packed_valid_reg <= '1';
                                    packed_last_reg  <= '1';
                                    current_state    <= WAIT_LOW_PIXEL;
                                else
                                    packed_valid_reg <= '0';
                                    packed_last_reg  <= '0';
                                    current_state    <= WAIT_HIGH_PIXEL;
                                end if;
                            else
                                packed_valid_reg <= '0';
                                packed_last_reg  <= '0';
                            end if;

                        -- Čekamo drugi (viši) piksel za kompletiranje 16-bitne reči
                        when WAIT_HIGH_PIXEL =>
                            if m_tvalid_dly = '1' then
                                packed_data_reg  <= in_px & px_low;
                                packed_valid_reg <= '1';
                                packed_last_reg  <= m_tlast_dly;
                                current_state    <= WAIT_LOW_PIXEL;
                            else
                                packed_valid_reg <= '0';
                                packed_last_reg  <= '0';
                            end if;

                    end case;
                else
                    current_state    <= WAIT_LOW_PIXEL;
                    px_low           <= (others => '0');
                    packed_data_reg  <= (others => '0');
                    packed_valid_reg <= '0';
                    packed_last_reg  <= '0';
                end if;
            end if;
        end if;
    end process pack_proc;

    packed_data  <= packed_data_reg;
    packed_valid <= packed_valid_reg;
    packed_last  <= packed_last_reg;

end architecture rtl;