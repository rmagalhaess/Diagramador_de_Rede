//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

int mouse;
int mouse_X;
int mouse_Y;

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
       mouse = 0;
       mouse_X = 0;
       mouse_Y = 0;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
        Canvas->MoveTo(20, 15);
	Canvas->LineTo(255, 82);

	TPoint Pt[7];

	Pt[0] = Point(20, 50);
	Pt[1] = Point(180, 50);
	Pt[2] = Point(180, 20);
	Pt[3] = Point(230, 70);
	Pt[4] = Point(180, 120);
	Pt[5] = Point(180, 90);
	Pt[6] = Point(20, 90);

	Canvas->Polyline(Pt, 1);                
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CirculoMouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
        Circulo->Pen->Width = 4;
        if (mouse == 1)
        {
                Circulo->Left = Circulo->Left - mouse_X + X;
                Circulo->Top  = Circulo->Top - mouse_Y + Y;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::QuadradoMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        Circulo->Pen->Width = 1;        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CirculoMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        if (Button == mbLeft)
        {
                mouse = 1;
                mouse_X = X;
                mouse_Y = Y;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CirculoMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
       mouse = 0;
       mouse_X = 0;
       mouse_Y = 0;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::VerticeMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        if (Button == mbLeft)
        {
                mouse = 1;
                mouse_X = X;
                mouse_Y = Y;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::VerticeMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
       mouse = 0;
       mouse_X = 0;
       mouse_Y = 0;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::VerticeMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        ((TShape *)Sender)->Pen->Width = 4;
        //Vertice->Pen->Width = 4;
        if (mouse == 1)
        {
                ((TShape *)Sender)->Parent->Left = ((TShape *)Sender)->Parent->Left - mouse_X + X;
                ((TShape *)Sender)->Parent->Top = ((TShape *)Sender)->Parent->Top - mouse_Y + Y;
                //pnl_Vertice->Left = pnl_Vertice->Left - mouse_X + X;
                //pnl_Vertice->Top  = pnl_Vertice->Top - mouse_Y + Y;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::pnl_VerticeMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    TShape *Aux;

    Aux = (TShape *)((TPanel *)Sender)->FindChildControl("Shape1");
    if (Aux != NULL)
    {
        Aux->Pen->Width = 1;
    }
    //((TShape *)Sender)->Pen->Width = 1;
    //Vertice->Pen->Width = 1;
}
//---------------------------------------------------------------------------

