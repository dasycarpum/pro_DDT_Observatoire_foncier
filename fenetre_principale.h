#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QButtonGroup>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>

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
    QButtonGroup *grp_granularite;
    Periode *periode;

    QwtPlot *graph_bati_cumul;
    QwtPlot *graph_bati_courant;

public:
    FenetrePrincipale(QWidget *parent = nullptr);
    ~FenetrePrincipale();

    void Affichage_tableau_occupation(const Algorithme *);
    void Affichage_tableau_evolution(const Algorithme *);
    void Affichage_graphique_bati_cumul(Algorithme *);
    void Affichage_graphique_bati_courant(Algorithme *);

public slots:
    void Gestion_granularite_territoire(QAbstractButton *);
    void Validation_des_saisies(bool);

};
#endif // FENETREPRINCIPALE_H
