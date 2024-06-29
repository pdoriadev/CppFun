#ifndef TEXTFINDER_H
#define TEXTFINDER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class TextFinder; }
QT_END_NAMESPACE

class TextFinder : public QMainWindow
{
    Q_OBJECT

public:
    TextFinder(QWidget *parent = nullptr);
    ~TextFinder();

private:
    Ui::TextFinder *ui;
};
#endif // TEXTFINDER_H
