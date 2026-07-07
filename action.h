#ifndef ACTION_H
#define ACTION_H

enum Action {

    /* -------------------------------- */
    /* Actions associated with the grid */

    /// Move to the next cell above (if possible) in the grid
    GRID_MOVE_UP,
    /// Move to the next cell below (if possible) in the grid
    GRID_MOVE_DOWN,
    /// Move to the next cell to the left (if possible) in the grid
    GRID_MOVE_LEFT,
    /// Move to the next cell to the right (if possible) in the grid
    GRID_MOVE_RIGHT,

    /* __________________________________ */




    /* ---------------------------------- */
    /* Actions associated with the state  */

    /// Switches the editing mode from LAYOUT <-> FILL
    STATE_SWITCH_EDITING_MODE,
    /// Swaps the fill direction from ACROSS <-> DOWN
    STATE_SWAP_FILL_DIRECTION,

    /* __________________________________ */





    /* ---------------------------------- */
    /*  	    	Other actions  	      */

    /// Opens a prompt for a filename to save the current grid to
    SAVE_TO_FILE,
    /// Opens a prompt for a file to load a grid from
    LOAD_FROM_FILE,
    /// Placeholder for there being no action to take
    NO_ACTION

    /* __________________________________ */
};


#endif // ACTION_H
