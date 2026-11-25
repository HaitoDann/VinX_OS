# Écran de connexion SDDM « vinx » (utilise les modules QML Vinx.Theme et Vinx.Controls installés).
install(DIRECTORY vinx DESTINATION "${CMAKE_INSTALL_DATADIR}/sddm/themes")
install(FILES vinx-sddm.conf DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/sddm.conf.d" RENAME 20-vinx-clavier.conf)
