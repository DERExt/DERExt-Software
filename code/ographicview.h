#ifndef GRAPHICVIEW
#define GRAPHICVIEW

#include "igraphicitem.h"

#include <QWidget>
#include <QGraphicsView>
#include <QDir>
#include <QtXml/QDomDocument>
#include <QFileDialog>
#include <QMouseEvent>
#include <QMenu>

class GraphicView : public QGraphicsView
{
    Q_OBJECT

    QString path;
    bool saved;
    QRubberBand * selection;
    QPoint origin;

private:
    void setName();

public:
    GraphicView(QWidget *parent);
    void setPath(QString path);
    QString getPath() const;
    QString getName() const;
    bool isSaved() const;
    void setSaved(bool saved);
    bool saveView();
    void save(bool &ok);
    void saveAs(bool &ok);
    IGraphicItem* getItem(QString name);
    QMenu *buildAlignMenu();
    void align(bool alignX, bool min);

public slots:
    void alignTop();
    void alignBottom();
    void alignLeft();
    void alignRight();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
};

#endif // GRAPHICVIEW

