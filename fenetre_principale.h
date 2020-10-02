#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>

#include "territoire.h"
#include "periode.h"
#include "algorithme.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FenetrePrincipale; }
QT_END_NAMESPACE

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT
    Ui::FenetrePrincipale *ui;

    Territoire *territoire;
    Periode *periode;

public:
    FenetrePrincipale(QWidget *parent = nullptr);
    ~FenetrePrincipale();

public slots:
    void Validation_des_saisies(bool);

};
#endif // FENETREPRINCIPALE_H
