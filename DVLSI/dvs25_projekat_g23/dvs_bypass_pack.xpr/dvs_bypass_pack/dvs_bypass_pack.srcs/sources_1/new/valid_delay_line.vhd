library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Provlaci 1-bitni "tag" (npr. valid_in_tag ili is_last_in_tag) kroz DEPTH
-- taktova, sinhronizovano SA pipeline enable-om (pipe_en).
-- Nakon tacno DEPTH aktivnih taktova, tag_out je isti bit koji je usao
-- kao tag_in, i garantovano je poravnat sa podatkom koji u istom taktu
-- izlazi iz convolution_core-a (jer prolazi kroz isti broj pipe_en
-- kvalifikovanih koraka).

entity valid_delay_line is
    generic (
        DEPTH : natural := 8   -- = PIPELINE_DELAY, koliko taktova acc_out kasni
    );
    port (
        clk         : in  std_logic;
        reset       : in  std_logic;
        
        pipe_en     : in  std_logic;   -- ISTI signal koji pomera convolution_core!
        config_mode : in  std_logic; 
        
        tag_in      : in  std_logic;   -- racunas ga NA ULAZU, iz pozicije piksela
        tag_out     : out std_logic    -- izlazi poravnat sa acc_out, DEPTH taktova kasnije
    );
end entity;

architecture rtl of valid_delay_line is

    type tag_mem_t is array (0 to DEPTH - 1) of std_logic;
    signal mem : tag_mem_t := (others => '0');
    attribute ram_style : string;
    attribute ram_style of mem : signal is "distributed";  -- LUTRAM, jeftino

    signal ptr : unsigned(31 downto 0) := (others => '0');  -- moze i manje bitova, ovo je "safe" sirina

begin

    process(clk)
        variable limit : integer;
    begin
        if rising_edge(clk) then
            if reset = '1' or config_mode = '1' then
                ptr     <= (others => '0');
                tag_out <= '0';
                mem     <= (others => '0');
            elsif pipe_en = '1' then
                limit := DEPTH - 1;

                -- PRVO procitaj STARU vrednost (upisanu pre DEPTH/DEPTH-1 taktova)
                tag_out <= mem(to_integer(ptr));

                --upisi novu vrednost na isto mesto
                mem(to_integer(ptr)) <= tag_in;

                -- pomeri pokazivac kruzno
                if ptr >= limit then
                    ptr <= (others => '0');
                else
                    ptr <= ptr + 1;
                end if;
            end if;
            -- kad je pipe_en='0', SVE stoji (i mem, i ptr, i tag_out)
        end if;
    end process;

end architecture rtl;