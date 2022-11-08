#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>

#define GAME_FILE "game.txt"
#define RESET "\033[0m"
#define BOLDRED "\033[1m\033[32m"

static int row, col;

/* Print symbolic grid  */
void print(bool **ptr)
{
    for(int i = 0; i < row; ++i)
    {
        for(int j = 0; j < col; ++j)
        {
            if(ptr[i][j])
                std::cout <<  BOLDRED << '*' << ' ';
            else
                std::cout << RESET << '-' << ' ';
        }
        std::cout << std::endl;
    }
}

/* Return true if grids are different */
bool diff(bool **grid1, bool **grid2) {

    for(int i = 0; i < row; ++i) {
        for(int j = 0; j < col; ++j) {
            if (grid1[i][j] == grid2[i][j])
                continue;
            else
                return true;
        }
    }

    return false;
}

/*
    Update ptr2 depends on ptr1
    return values:
        0 - all good
        1 - err! two grids alike (stagnation)
        2 - err! no alive cells
*/
int updateGrid(bool **ptr1, bool **ptr2, int &live) {

    int alive = 0,
    row_beg = 0, row_end = 0,
    col_beg = 0, col_end = 0;

    for(int r = 0; r < row; ++r)
    {
        row_beg = r - 1;
        row_end = r + 1;

        for(int c = 0; c < col; ++c)
        {
            alive = 0;
            col_beg = c - 1;
            col_end = c + 1;

            if ( r == 0 ) row_beg = 0;
            if ( c == 0 ) col_beg = 0;
            if ( row_end == row ) row_end = row-1;
            if ( col_end == col ) col_end = col-1;

            for(int i = row_beg; i <= row_end; ++i)
            {
                for(int j = col_beg; j <= col_end; ++j) {
                    if(r == i && c == j) continue;
                    if(ptr1[i][j]) alive++;
                }
            }

            if(ptr1[r][c]) {
                if(alive == 2 || alive == 3)
                    ptr2[r][c] = true;
                else
                    ptr2[r][c] = false;
            } else {
                if(alive == 3)
                    ptr2[r][c] = true;
                else 
                    ptr2[r][c] = false;
            }
        }
    }

    bool different = diff(ptr1, ptr2);

    for(int i = 0; i < row; ++i)
    {
        for(int j = 0; j < col; ++j)
        {
            if (ptr1[i][j]) ++live;
            ptr1[i][j] = ptr2[i][j];
        }
    }

    if (live == 0) return 2;
    if(!different) return 1;
    return 0;
}

int main()
{
    std::ifstream in(GAME_FILE);
    if (!in.is_open()) {
        std::cout << "Error, file " << GAME_FILE << " not found\n";
        return -1;
    }

    std::string line;
    getline(in, line);

    char *str = const_cast<char *>(line.c_str());
    row = std::stoi(strtok(str, " "));
    col = std::stoi(strtok(nullptr, " "));

    bool **grid1 = new bool*[row];
    bool **grid2 = new bool*[row];

    for(int i = 0; i < row; ++i)
    {
        grid1[i] = new bool[col];
        grid2[i] = new bool[col];

        memset(grid2[i], 0, col * sizeof(bool));
        memset(grid1[i], 0, col * sizeof(bool));
    }
    
    int alive = 0;
    while(getline(in, line))
    {
        int r = std::stoi(strtok(const_cast<char *>(line.c_str()), " "));
        int c = std::stoi(strtok(nullptr, " "));
        grid1[r][c] = true;
        grid2[r][c] = true;
        ++alive;
    }

    int gen = 1, ret = 0;
    while(alive > 0)
    {
        system("clear");
        print(grid1);

        alive = 0;
        ret = updateGrid(grid1, grid2, alive);
        
        std::cout << "Generation: "  << gen++ << ". " 
             << "Alive cells: " << alive << std::endl;

        if (ret == 2)
            std::cout << "All cells are dead. Game over!\n";

        if (ret == 1) {
            std::cout << "The world has stagnated. Game over!\n";
            break;
        }

        sleep(1);
    }
    
    for(int i = 0; i < row; ++i) {
        delete []grid1[i];
        delete []grid2[i];
    }

    delete []grid1;
    delete []grid2;
    in.close();

    return 0;
}
