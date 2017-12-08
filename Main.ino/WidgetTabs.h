

void widtab_initWidget(WidgetTabs* w,int tabNumber, void (*callbackTouchEvent)(int tabNumber, int prevTabNumber) );  
void widtab_setTabNumber(WidgetTabs* w, int tabNumber);
void widtab_startDrawingTabs(WidgetTabs* w);
int widtab_runDrawingTabs(WidgetTabs* w);
int widtab_touchEvent(WidgetTabs* w,int x,int y);



