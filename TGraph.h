/** \file
    \brief Contains the TGraph class and its helper classes
*/
#ifndef _T_GRAPH_H_
#define _T_GRAPH_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/notebook.h>
#include <wx/grid.h>

typedef vector <string> TVS ;
typedef vector <float> floatArray ;
typedef vector <TVS> stringField ;

class TGraph ;
class TGraphData ;
class TGraphScale ;
class TGraphDisplay ;

#define GRAPH_DRAW_SCALES 1
#define GRAPH_DRAW_MAP 2
#define GRAPH_DRAW_MINI 4
#define GRAPH_DRAW_ALL (GRAPH_DRAW_SCALES|GRAPH_DRAW_MAP)

/** \brief The class containing xy-data for graph drawing
*/
class TGraphData
    {
    public :
     TGraphData ( TGraphDisplay * const d = NULL ) ; ///< Constructor
     void Add ( const float x , const float y ) ; ///< Add a XY pair
     void SetScales ( TGraphScale * const _sx , TGraphScale * const _sy ) ; ///< Assign scales to this gprah
     void AutoScale () ; ///< Size scales to (at least) fit the data in this graph
     unsigned long GetCount () const ; ///< Returns the number of XY pairs
     int Distance ( const wxPoint &pt ) const ; ///< Returns the minimum distance of the given point to the graph curve

     void drawit ( wxDC &dc ) ; ///< Draws the graph in the given drawing context
     void DrawSymbol ( wxDC &dc , const wxString& symbol , const int x , const int y , const int size = 4 ) const ; ///< Draws a point symbol (if any)

     wxString name ; ///< Name of the graph
     wxString pointStyle ; ///< Style of the point (used in DrawSymbol)
     floatArray dx , dy ;
    TGraphDisplay *display ;
    TGraphScale *sx , *sy ;
    wxColour col ; ///< The color of this graph
    vector <wxPoint> drawn ; ///< Cache of last drawn points in pixels
    bool selected ; ///< Is this graph selected (by mouse)?
    } ;

/**    \brief The class containing a scale used in the graph
*/
class TGraphScale
    {
    public :
    TGraphScale ( const float _min = 0 ,
                  const float _max = 0 ,
                  const bool _horizontal = false ,
                  const bool _left = true ,
                  const wxString& _name = _T("") ,
                  const wxString& _unit = _T("") ,
                  const wxColour& _col = *wxBLACK ,
                  const wxString& _type = _T("linear") ) ; ///< Constructor
    int GetWidth () const ; ///< Returns height/width in pixels
    void drawit ( wxDC &dc , wxRect &r , wxRect &inner ) ; ///< Draws the scale, adjusts the inner rectangle
    void DrawMark ( wxDC &dc , const float p , wxRect &ir , wxString text , const bool big = false ) ; ///< Draws the red marker for cursor position
    int GetRealCoord ( const float f , const wxRect &inner ) const ; ///< Converts float value to pixel
    float GetVirtualCoordinate ( int i , const wxRect &inner ) const ; ///< Converts pixel to float
    wxRect CalcInternalRect ( wxRect &r ) const ; ///< Returns the internal rectangle
    void Drag ( int delta ) ; ///< Drags the scale into a direction (pixel)
    float GetTotalWidth () const { return max - min ; } ///< Returns the height/width in arb. units
    float GetVisibleWidth () const { return top - bottom ; } ///< Returns the visible height/width in arb. units

    wxRect outline ;
    wxRect last_inner ;
    float min , max ;
    float top , bottom ;
    bool horizontal , left , selected ;
    wxString name , unit , type ;
    wxColour col ;
    int mark ;
    bool show_mark ;
    } ;

/** \brief The display class, containing graphs and scales; essentially, a "tab"
*/
class TGraphDisplay : public wxPanel
    {
    public :
    TGraphDisplay ( wxWindow *parent , int id = -1 ) ; ///< Constructor
    ~TGraphDisplay () ; ///< Destructor
    void init () ; ///< Initialization

    stringField readTextfile ( const wxString& filename ) ; ///< Converts a CSV file into s stringField
    unsigned char *readRawData ( const wxString& filename , long &l ) ; ///< Reads raw binary data
    void setupPhotometerGraph ( const stringField &sf ) ; ///< Reads CSV-based photometer data
    void setupFluorimeterGraph ( const stringField &sf ) ; ///< Reads CSV-based fluorimeter data
    void setupXYpair ( const stringField& sf ) ; ///< Reads plain XY-pair CSV data
    void setupIPCfile ( const wxString& filename ) ; ///< Reads IPC data
    void setupRawFPLC ( const wxString& filenamebase ) ; ///< EXPERIMENTAL Reads raw BioRad FPLC data
    void setupBioFormat ( const wxString& filenamebase ) ; ///< EXPERIMENTAL Reads BIO FPLC format
    void setupBioCSVFormat ( const stringField& sf ) ; ///< EXPERIMENTAL Reads BIO CSV FPLC format
    void setupDUF ( const wxString& filenamebase ) ; ///< Reads Beckman DUF format
    void addNewGraph ( const stringField& sf , const wxString& title , TGraphScale *sx , TGraphScale*sy , int startrow = 0 ) ; ///< Adds a TGraphData
    void addRawData ( unsigned char *d , long l , const wxString& title ) ;
    void addRawData2 ( unsigned char *d , long l , const wxString& title ) ;
    wxString tryall ( const wxString& filename ) ; ///< Tries all formats, pre-selects the "best fit"
    void SetupDummy () ; ///< For testing only
    bool SetupFromFile ( const wxString& filename ) ; ///< Calls tryall, display dialog, eventually opens file
    void SetZoom ( int _zx , int _zy ) ; ///< Sets the zoom factor
    void UpdateDisplay () ; ///< Refresh

    void AutoScale () ; ///< Adjusts the scales to fit the graphs
    void drawit ( wxDC &dc , const int mode = GRAPH_DRAW_ALL ) ; ///< Draws the graphs and scales in the device context
    void showLegend ( wxDC &dc ) ; ///< Draws the legend
    void showMiniature ( wxDC &dc ) ; ///< Draws the miniature preview (in zoomed mode)
    void showDraggingRect ( wxDC &dc ) ;

    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
    virtual void OnEvent(wxMouseEvent& event) ; ///< Mouse event handler
    virtual void OnSwapSides(wxCommandEvent &event) ; ///< "Switch scales left/right or top/bottom" event handler
    virtual void OnCopyAsImage(wxCommandEvent &event) ; ///< "Copy as image" event handler
    virtual void OnSaveAsImage(wxCommandEvent &event) ; ///< "Save as image" event handler

    vector <TGraphScale*> scales ; ///< The scales
    vector <TGraphData*> data ; ///< The graphs (data)
    TGraph *g ; ///< The calling TGraph
    static wxColour prettyColor ; ///< A pretty blue
    wxArrayString scaleTypes ;
    wxArrayString colors , styles ;

    private :
    bool IsSetupComplete() const ; ///< Don't draw before setup is complete (divsion by zero and other ugliness)
    void DrawIntoBitmap ( wxBitmap &bmp ) ; ///< Draws the current display into a wxBitmap instead of the display (for copying and saving)

    bool setupCompleted ; ///< Is setup complete?
    TGraphScale *old_scale ; ///< The last scale
    TGraphData *old_data ; ///< The last graph

    wxRect inner ; ///< The place where the data is drawn
    wxRect lr ; ///< The legend rectangle
    int zx , zy ;
    wxPoint mouse_pos ;
    wxRect draggingRect ;

    DECLARE_EVENT_TABLE()
    } ;

/**    \class TGraph
    \brief The graph drawing module
*/
class TGraph : public ChildBase
    {
    public :
    TGraph(wxWindow *parent, const wxString& title) ; ///< Constructor
    ~TGraph () ; ///< Destructor

    void initme () ; ///< Initialization
    virtual wxString getName () const ; ///< Returns the module name

    virtual void OnZoomX(wxScrollEvent& event); ///< Zoom event handler
    virtual void OnZoomY(wxScrollEvent& event); ///< Zoom event handler
    virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler
    virtual void OnCopy(wxCommandEvent& event) ; ///< Copy event handler
    virtual void OnSave(wxCommandEvent& event) ; ///< Save event handler
    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler

    TGraphDisplay *gd ; ///< Pointer to the display class (a tab)

    private :
    friend class TGraphDisplay ;
    wxNotebook *nb ; ///< Pointer to the wxNotebook structure that holds the submodules
    wxSlider *zoom_x , *zoom_y ;
    wxCheckBox *zoom_linked ;

    DECLARE_EVENT_TABLE()
    } ;


#endif

