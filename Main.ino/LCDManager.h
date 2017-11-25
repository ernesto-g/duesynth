struct S_Widget
{
  int type;
  int x;
  int y;
  int w;
  int h;  
  int state;
  int touchAreaX0;
  int touchAreaY0;
  int touchAreaX1;
  int touchAreaY1;  
};
typedef struct S_Widget Widget;

struct S_WidgetVerticalControl
{
  Widget obj;
  int value;
  int prevValue;
};
typedef struct S_WidgetVerticalControl WidgetVerticalControl;

struct S_WidgetButton
{
  Widget obj;
  char text[16];
  unsigned char* pBitmapIdle;
  unsigned char* pBitmapPressed;    
};
typedef struct S_WidgetButton WidgetButton;

struct S_WidgetImage
{
  Widget obj;
  unsigned char* pBitmap;
};
typedef struct S_WidgetImage WidgetImage;

#define WIDGET_TYPE_VERTICAL_CONTROL  0
#define WIDGET_TYPE_BUTTON            1
#define WIDGET_TYPE_IMAGE             2



#define WIDGET_STATE_IDLE     0
#define WIDGET_STATE_REDRAW   1
#define WIDGET_STATE_PRESSED  2
#define WIDGET_STATE_HIDDEN   4


void win_init(void);
void win_addWidget(int i, Widget* w);
void win_drawAll(void);
void win_touchManager(void);
void win_sysTick(void);




