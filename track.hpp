#ifndef TRACK_HPP
#define TRACK_HPP

class Track
{
public:
    enum State
    {
        SELECT,
        PLACE,
        MOVE,
        ROTATE,
        FREECAM
    };
private:
    static Track*   m_track;
    
    State           m_state;

    bool            m_grid_on;

    Track() {};

public:
    static Track* getTrack();
    void          init();
    void          setState(State state);
    void          setGrid(bool grid_on);
    void          changeGridDensity(int dir);

    bool          isGridOn()        { return m_grid_on; }
};

#endif
