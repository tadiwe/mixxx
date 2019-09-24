#pragma once

#include <memory>

#include "test/mixxxtest.h"

#include "database/mixxxdb.h"
#include "library/trackcollection.h"
#include "util/db/dbconnectionpooler.h"
#include "util/db/dbconnectionpooled.h"
#include "track/globaltrackcache.h"

const bool kInMemoryDbConnection = true;

class LibraryTest : public MixxxTest,
    public virtual /*implements*/ GlobalTrackCacheSaver {

  public:
    void saveEvictedTrack(Track* pTrack) noexcept override {
        m_pTrackCollection->exportTrackMetadata(pTrack);
        m_pTrackCollection->saveTrack(pTrack);
    }

  protected:
    LibraryTest()
        : m_mixxxDb(config(), kInMemoryDbConnection),
          m_dbConnectionPooler(m_mixxxDb.connectionPool()),
          m_dbConnection(mixxx::DbConnectionPooled(m_mixxxDb.connectionPool())),
          m_pTrackCollection(std::make_unique<TrackCollection>(config())) {
        MixxxDb::initDatabaseSchema(m_dbConnection);
        m_pTrackCollection->connectDatabase(m_dbConnection);
        GlobalTrackCache::createInstance(this);
    }
    ~LibraryTest() override {
        m_pTrackCollection->disconnectDatabase();
        m_pTrackCollection.reset();
        // With the track collection all remaining track references
        // should have been dropped before destroying the cache.
        GlobalTrackCache::destroyInstance();
    }

    mixxx::DbConnectionPoolPtr dbConnectionPool() const {
        return m_mixxxDb.connectionPool();
    }

    QSqlDatabase dbConnection() const {
        return m_dbConnection;
    }

    TrackCollection* collection() {
        return m_pTrackCollection.get();
    }

  private:
    const MixxxDb m_mixxxDb;
    const mixxx::DbConnectionPooler m_dbConnectionPooler;
    QSqlDatabase m_dbConnection;
    std::unique_ptr<TrackCollection> m_pTrackCollection;
};
