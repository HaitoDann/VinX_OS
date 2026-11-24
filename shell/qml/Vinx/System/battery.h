#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

// Batterie via UPower (périphérique d'affichage agrégé). present = false sur un poste fixe.
class Battery : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool present READ present NOTIFY changed)
    Q_PROPERTY(int percentage READ percentage NOTIFY changed)
    Q_PROPERTY(bool charging READ charging NOTIFY changed)
    // Santé : capacité actuelle / capacité d'origine, en % (-1 si inconnue).
    Q_PROPERTY(int health READ health NOTIFY changed)
    // Secondes avant vide (sur batterie) ou avant charge complète (en charge) ; 0 si inconnu.
    Q_PROPERTY(qint64 timeRemaining READ timeRemaining NOTIFY changed)
    // Limite de charge du matériel (charge_control_end_threshold), -1 si non prise en charge.
    Q_PROPERTY(int chargeLimit READ chargeLimit NOTIFY changed)

public:
    explicit Battery(QObject *parent = nullptr);
    bool present() const { return m_present; }
    int percentage() const { return m_percentage; }
    bool charging() const { return m_charging; }
    int health() const { return m_health; }
    qint64 timeRemaining() const { return m_timeRemaining; }
    int chargeLimit() const;

Q_SIGNALS:
    void changed();

private Q_SLOTS:
    void refresh();

private:
    bool m_present = false;
    int m_percentage = 0;
    bool m_charging = false;
    int m_health = -1;
    qint64 m_timeRemaining = 0;
};
