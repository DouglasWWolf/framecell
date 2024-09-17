//=============================================================================
// frame-cell - Converts a frame-offset to a cell number of vice-versa
//
// Author: Doug Wolf
//=============================================================================


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
using std::string;

const int ROWS_PER_FRAME    = 256;
const int SUBROWS_PER_ROW   = 8;
const int CYCLES_PER_SUBROW = 32;
const int CELLS_PER_CYCLE   = 64;
const int CELLS_PER_ROW     = SUBROWS_PER_ROW * CYCLES_PER_SUBROW * CELLS_PER_CYCLE;
const int CELLS_PER_FRAME   = CELLS_PER_ROW * ROWS_PER_FRAME;


int cell[CELLS_PER_FRAME];

bool xlate_offset_to_cell;
bool xlate_cell_to_offset;
int  xlate_index = -1;


//=============================================================================
// build_map() - Creates the cell-map.
//
// Each offset in the cell-map will contain the cell number post-reordering
//=============================================================================
void build_map()
{
    int idx = 0;

    for (int row = 0; row < ROWS_PER_FRAME; ++row)
    {
        int initial_row_cell = row * CELLS_PER_ROW;

        for (int subrow = 0; subrow < SUBROWS_PER_ROW; ++subrow)
        {
            int initial_subrow_cell = initial_row_cell + subrow;

            for (int cycle = 0; cycle < CYCLES_PER_SUBROW; ++cycle)
            {
                int initial_cycle_cell = initial_subrow_cell + 8 * cycle;

                for (int offs = 0; offs < CELLS_PER_CYCLE; ++offs)
                {
                    cell[idx++] = initial_cycle_cell + offs * 256;                        
                }
            }
        }        
    }
}
//=============================================================================


//=============================================================================
// This prints the map of the first row of cells
//=============================================================================
void print_map()
{
    for (int i=0; i<CELLS_PER_FRAME; ++i)
    {
        if (i%64 == 0) printf("\n");
        printf(" %5i,", cell[i]);
    }

    printf("\n");
}
//=============================================================================


//=============================================================================
// usage() - Show usage and exit
//=============================================================================
void usage()
{
    printf("framecell -cell <cell_number>\n");
    printf("  -- or --\n");
    printf("framecell -offset <frame_offset>\n");
    exit(1);
}
//=============================================================================


//=============================================================================
// parse_command_line()
//=============================================================================
void parse_command_line(const char** argv)
{
    xlate_cell_to_offset = false;
    xlate_offset_to_cell = false;
    xlate_index          = -1;

    // Is there a command name?
    if (argv[1] == nullptr) usage();

    // If there is an index given on the command line, parse it
    if (argv[1] && argv[2]) xlate_index = atoi(argv[2]);

    // Fetch the command from the command line
    string command = argv[1];

    // Are we printing the cell map?
    if (command == "-print")
    {
        print_map();
        exit(0);
    }

    // Are we translating a cell number to an offset?
    xlate_cell_to_offset = (command == "-cell");
    
    // Are we translating an offset to a cell number?
    xlate_offset_to_cell = (command == "-offset");

    // If the user left off a switch, show usage
    if (!xlate_offset_to_cell && !xlate_cell_to_offset) usage();

    // If the user specified an invalid index, show usage
    if (xlate_index < 0 || xlate_index >= CELLS_PER_FRAME) usage();
}
//=============================================================================



//=============================================================================
// main() - execution begins here
//=============================================================================
int main(int argc, const char** argv)
{

    // Build the map off offset --> cell #
    build_map();

    // Parse the command line
    parse_command_line(argv);

    // Are we translating a frame-offset to a cell number?
    if (xlate_offset_to_cell)
    {
        printf("Frame offset %i contains cell %i\n", xlate_index, cell[xlate_index]);
        exit(0);
    }

    // If we get here, we're translating a cell number to an offset
    for (int i=0; i<CELLS_PER_FRAME; ++i)
    {
        if (cell[i] == xlate_index)
        {
            printf("Cell %i is at frame offset %i\n", xlate_index, i);
            exit(0);            
        }

    }

        
    return 0;
}
//=============================================================================