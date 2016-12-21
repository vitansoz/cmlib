#include "cmmediascanner.h"
#include <QDebug>
#include <QDir>

CMMediaScanner::CMMediaScanner(QObject *parent) : QObject(parent)
{
    m_ticker.setSingleShot(true);
    m_ticker.setInterval(100);
    connect(&m_ticker, SIGNAL(timeout()), this, SLOT(scanLoop()));
}

void CMMediaScanner::setFilters(const QStringList &filter)
{
    m_filter=filter;
}

bool CMMediaScanner::addPath(const QString path)
{
    if (m_paths.contains(path))
        return true;

    QDir p(path);
    if (p.makeAbsolute()==false)
        return false;

    if (p.exists(path)==false)
        return false;

    m_paths.append(path);
    qDebug() << path;

    return true;
}

void CMMediaScanner::clearPaths()
{
    m_paths.clear();
}

bool CMMediaScanner::scanAsync()
{
    if (m_paths.isEmpty())
        return false;

    if (scan(m_filelist, true))
        m_ticker.start();

    return true;
}

void CMMediaScanner::scanLoop()
{
    if (scan(m_filelist, false))
        m_ticker.start();
}

/**
 * @brief CMMediaScanner::scanDirectories
 * @param paths
 * @param filter
 * @param list
 * @return
 */
bool CMMediaScanner::scan(QStringList &list, bool fromStart)
{
    bool r;

    if (m_paths.isEmpty())
        return false;

    if (m_pathsleft.isEmpty() || fromStart)
        m_pathsleft=m_paths;

    const QString path=m_pathsleft.takeFirst();
    QDirIterator it(path, m_filter, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    emit scanning(path);

    while (it.hasNext()) {
        QString f;
        QFileInfo info;

        f=it.next();

        info=it.fileInfo();
        if (info.isFile())
            list.append(f);
        else if (info.isDir()) {
            m_pathsleft.append(f);
        }
    }

    r=!m_pathsleft.isEmpty();
    if (r)
        emit scanningDone();

    return r;
}


