#pragma once

#include <QDesktopServices>
#include <QFileInfo>
#include <QObject>
#include <QPdfDocument>
#include <QPdfSelection>
#include <QProcess>
#include <QQuickImageProvider>
#include <QUrl>

// Lecteur PDF de la Visionneuse VinX (ADR 0052) : document ouvert avec le module PDF de Qt,
// pages rendues à la demande par le fournisseur d'images « pdfpage ».
class PdfDoc : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString path READ path NOTIFY changed)
    Q_PROPERTY(QString name READ name NOTIFY changed)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY changed)
    Q_PROPERTY(bool ready READ ready NOTIFY changed)
    Q_PROPERTY(QString error READ error NOTIFY changed)

public:
    static PdfDoc *instance() { static PdfDoc doc; return &doc; }
    QPdfDocument *document() { return &m_doc; }

    bool open(const QString &file)
    {
        m_path = QFileInfo(file).absoluteFilePath();
        const auto err = m_doc.load(m_path);
        m_error = err == QPdfDocument::Error::None ? QString()
            : err == QPdfDocument::Error::IncorrectPassword ? tr("Ce document est protégé par un mot de passe.")
            : tr("Impossible d'ouvrir ce document PDF.");
        Q_EMIT changed();
        return m_error.isEmpty();
    }
    QString path() const { return m_path; }
    QString name() const { return QFileInfo(m_path).fileName(); }
    int pageCount() const { return m_doc.pageCount(); }
    bool ready() const { return m_error.isEmpty() && m_doc.pageCount() > 0; }
    QString error() const { return m_error; }

    // Taille de la page en points (1/72 de pouce).
    Q_INVOKABLE QSizeF pageSize(int page) const { return m_doc.pagePointSize(page); }

    // Première page, à partir de « from » (en boucle), dont le texte contient « text » ; -1 sinon.
    Q_INVOKABLE int find(const QString &text, int from)
    {
        const QString needle = text.trimmed();
        if (needle.isEmpty() || pageCount() == 0)
            return -1;
        for (int i = 0; i < pageCount(); ++i) {
            const int page = (from + i) % pageCount();
            if (m_doc.getAllText(page).text().contains(needle, Qt::CaseInsensitive))
                return page;
        }
        return -1;
    }
    // Formulaires, annotations, impression : Firefox (pdf.js) les gère.
    Q_INVOKABLE void openInBrowser() const
    {
        for (const char *b : {"firefox-esr", "firefox", "xdg-open"})
            if (QProcess::startDetached(QString::fromLatin1(b), {m_path}))
                return;
    }
    Q_INVOKABLE void openFolder() const { QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_path).absolutePath())); }

Q_SIGNALS:
    void changed();

private:
    QPdfDocument m_doc;
    QString m_path, m_error;
};

// image://pdfpage/<page> : rendu à la largeur demandée (sourceSize.width).
class PdfPageProvider : public QQuickImageProvider
{
public:
    PdfPageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}
    QImage requestImage(const QString &id, QSize *size, const QSize &requested) override
    {
        QPdfDocument *doc = PdfDoc::instance()->document();
        const int page = id.section(QLatin1Char('/'), 0, 0).toInt();
        const QSizeF pt = doc->pagePointSize(page);
        if (pt.isEmpty())
            return {};
        const int width = requested.width() > 0 ? requested.width() : int(pt.width() * 2);
        const QSize px(width, qRound(width * pt.height() / pt.width()));
        QImage img = doc->render(page, px);
        if (size)
            *size = img.size();
        return img;
    }
};
