#ifndef DATA_H
#define DATA_H


class Data
{
public:
    Data();
    float version = 0.01;
    bool redrawOutput=false;
    bool redrawInput=false;
    bool redrawFileList = false;
    bool abort = false;
    float percent = 0;
    unsigned char currentColor = 0;
    static Data data;
    void Redraw();
};

#endif // DATA_H
