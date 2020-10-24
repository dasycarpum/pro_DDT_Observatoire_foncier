# Introduction 
Sur base des fichiers MAJIC de la DGFiP, l’application de type IHM fournit, pour un périmètre territorial donné, 2 informations :
1. l’occupation du sol au temps t, en 2 classes distinctes : les ENAF d’un côté et les espaces artificialisés de l’autre ; ces derniers sont répartis entre bâti (logement, activité) et non bâti (infrastructures, espaces d’agrément, …)

2. l’évolution de l’artificialisation des sols, basée sur l’analyse de l’année de 1ère construction des parcelles, dont on déduit par rétropolation la consommation foncière (les périodes d’observations sont paramétrables).

# Environnement de développement
    ▪ Langage : C++11
    ▪ Framework : Qt (version 5.12.6)
    ▪ IDE : Qt Creator (version 4.10.2)
    ▪ Bibliothèque complémentaire : qwt (version 6.1.5)
    ▪ Gestionnaire de versions du code : Git (version 2.28.0)
    ▪ Plateforme applicative : Azure Devops sous
         https://dev.azure.com/dasycarpum-pro-DDT/Observatoire%20du%20foncier%2054
