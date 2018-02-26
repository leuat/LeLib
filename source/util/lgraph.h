#pragma once
#include "spline.h"

using namespace std;

#include <QVector>
#include <QPointF>
#include "util.h"
#include <QString>



class LGraph {
public:
    QVector<float> m_index;
    QVector<float> m_value;

    float m_minx, m_maxx;
    float m_miny, m_maxy;
    float m_delta;
    double m_mean, m_std;


    float m_meanY, m_stdY;

    QString m_name;

    int m_noBins;

    LGraph() {
    }

    void normalizeArea();

    void getMinMaxY();


    int getIndex(float xval) {
        float s = (xval - m_minx)/(m_maxx - m_minx);
        s*=m_noBins;
        if (s>=0 && s<m_noBins)
            return s;
        return -1;
    }

    float get(float xval) {
        int idx = getIndex(xval);
        if (idx!=-1)
            return m_value[idx];
        return -1;
    }
    void set(float xval, float value) {
        int idx = getIndex(xval);
        if (idx!=-1)
            m_value[idx] = value;
    }
    void add(float xval, float val) {
        int idx = getIndex(xval);
        if (idx!=-1)
            m_value[idx] += val;
    }

    void Initialize(int bins, float xmin, float xmax) {
        m_noBins = bins;
        m_minx = xmin;
        m_maxx = xmax;
        m_delta = (xmax - xmin)/bins;
        m_value.resize(bins);
        m_index.resize(bins);
        for (int i=0;i<bins;i++) {
            m_value[i] = 0;
            m_index[i] = xmin + m_delta*i;
        }

    }

//    void CalculateStatistics();

    void Normalize();


    void LoadText(QString Filename);
    void SaveText(QString Filename);

    void Mean();
    void Std();


};
