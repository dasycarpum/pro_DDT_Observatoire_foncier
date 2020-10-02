#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Initialisation du territoire d'étude */
    ui->radioButton_commune->setChecked(true);
    QStringList liste_geographies = Territoire::Liste_libelles_geographies("Commune");
    ui->comboBox_geographie->addItems(liste_geographies);

    /* Initialisation de la période d'observation */
    Periode *p = new Periode();
    p->Dernier_millesime_majic();
    p->Ajustement_automatique_annee_debut();
    ui->spinBox_an_debut->setValue(p->Annee_debut());
    ui->spinBox_an_fin->setValue(p->Annee_fin());
    ui->spinBox_an_pas->setValue(p->Pas_de_temps());

}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}

