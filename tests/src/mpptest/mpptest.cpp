/*
# PostgreSQL Database Modeler (pgModeler)
#
# (c) Copyright 2006-2026 - Raphael Araújo e Silva <raphael@pgmodeler.io>
#
# The complete text of GPLv3 is at LICENSE file on source code root directory.
# Also, you can get the complete GNU General Public License at <http://www.gnu.org/licenses/>
*/

#include <QtTest/QtTest>
#include <QTemporaryDir>
#include "coreutilsns.h"
#include "databasemodel.h"
#include "table.h"
#include "column.h"
#include "schema.h"
#include "pgmodelerunittest.h"

// Variadic so expressions containing commas can be passed
#define EXPECT_ERROR(code, ...) \
	do { \
		bool thrown = false; \
		try { __VA_ARGS__; } \
		catch(Exception &e) { thrown = (e.getErrorCode() == code); } \
		QVERIFY(thrown); \
	} while(0)

class MppTest: public QObject, public PgModelerUnitTest {
	Q_OBJECT

	private:
		Table *createTable(DatabaseModel &model);

	public:
		MppTest() : PgModelerUnitTest(SCHEMASDIR) {}

	private slots:
		void cleanup();
		void plainPostgreSQLIgnoresMppClauses();
		void greenplum6UsesReloptions();
		void greenplum7UsesAccessMethod();
		void cloudberryReplicatedAndRandom();
		void hashRequiresColumns();
		void missingKeyColumnRaisesError();
		void xmlRoundTrip();
		void saveAndLoadModel();
};

Table *MppTest::createTable(DatabaseModel &model)
{
	try
	{
	Table *tab = new Table;
	Column *col = nullptr;

	tab->setName("sales");
	Schema *sch = new Schema;
	sch->setName("sch_mpp");
	model.addObject(sch);
	tab->setSchema(sch);

	for(auto &name : { "id", "region" })
	{
		col = new Column;
		col->setName(name);
		col->setType(PgSqlType("integer"));
		tab->addColumn(col);
	}

	model.addObject(tab);
	return tab;
	}
	catch(Exception &e)
	{
		qWarning() << e.getExceptionsText();
		throw;
	}
}

void MppTest::cleanup()
{
	BaseObject::setServerFlavor(PgSqlVersions::FlavorPostgreSQL);
}

void MppTest::plainPostgreSQLIgnoresMppClauses()
{
	DatabaseModel model;
	Table *tab = createTable(model);

	tab->setDistribution(Table::DistHash, { "id" });
	tab->setStorage(Table::StorageAoColumn, "zstd", 3);

	BaseObject::setServerFlavor(PgSqlVersions::FlavorPostgreSQL);
	QString sql = tab->getSourceCode(SchemaParser::SqlCode);

	QVERIFY(!sql.contains("DISTRIBUTED"));
	QVERIFY(!sql.contains("appendonly"));
	QVERIFY(!sql.contains("USING"));
}

void MppTest::greenplum6UsesReloptions()
{
	DatabaseModel model;
	Table *tab = createTable(model);

	tab->setDistribution(Table::DistHash, { "id", "region" });
	tab->setStorage(Table::StorageAoColumn, "zlib", 5);

	BaseObject::setServerFlavor(PgSqlVersions::FlavorGreenplum6);
	QCOMPARE(BaseObject::getPgSQLVersion(), PgSqlVersions::PgSqlVersion94);

	QString sql = tab->getSourceCode(SchemaParser::SqlCode);

	QVERIFY(sql.contains("WITH (appendonly=true, orientation=column, compresstype=zlib, compresslevel=5)"));
	QVERIFY(sql.contains("DISTRIBUTED BY (id, region)"));
	QVERIFY(!sql.contains("USING"));
}

void MppTest::greenplum7UsesAccessMethod()
{
	DatabaseModel model;
	Table *tab = createTable(model);

	tab->setDistribution(Table::DistHash, { "id" });
	tab->setStorage(Table::StorageAoRow, "zstd", 3);

	BaseObject::setServerFlavor(PgSqlVersions::FlavorGreenplum7);
	QCOMPARE(BaseObject::getPgSQLVersion(), PgSqlVersions::PgSqlVersion120);

	QString sql = tab->getSourceCode(SchemaParser::SqlCode);

	QVERIFY(sql.contains("USING ao_row"));
	QVERIFY(sql.contains("WITH (compresstype=zstd, compresslevel=3)"));
	QVERIFY(sql.contains("DISTRIBUTED BY (id)"));
	QVERIFY(!sql.contains("appendonly"));
}

void MppTest::cloudberryReplicatedAndRandom()
{
	DatabaseModel model;
	Table *tab = createTable(model);

	BaseObject::setServerFlavor(PgSqlVersions::FlavorCloudberry);
	QCOMPARE(BaseObject::getPgSQLVersion(), PgSqlVersions::PgSqlVersion140);

	tab->setDistribution(Table::DistReplicated);
	QVERIFY(tab->getSourceCode(SchemaParser::SqlCode).contains("DISTRIBUTED REPLICATED"));

	tab->setDistribution(Table::DistRandom);
	QVERIFY(tab->getSourceCode(SchemaParser::SqlCode).contains("DISTRIBUTED RANDOMLY"));
}

void MppTest::hashRequiresColumns()
{
	Table tab;
	QStringList cols = { "id" };

	// Hash without key columns
	EXPECT_ERROR(ErrorCode::InvDistributionKeys, tab.setDistribution(Table::DistHash));

	// Columns informed for a policy that does not accept them
	EXPECT_ERROR(ErrorCode::InvDistributionKeys, tab.setDistribution(Table::DistRandom, cols));

	// Unknown policy
	EXPECT_ERROR(ErrorCode::InvDistributionKeys, tab.setDistribution("bogus"));
}

void MppTest::missingKeyColumnRaisesError()
{
	DatabaseModel model;
	Table *tab = createTable(model);

	tab->setDistribution(Table::DistHash, { "nonexistent" });
	BaseObject::setServerFlavor(PgSqlVersions::FlavorGreenplum7);

	EXPECT_ERROR(ErrorCode::InvDistributionKeyColumn, tab->getSourceCode(SchemaParser::SqlCode));
}

void MppTest::xmlRoundTrip()
{
	DatabaseModel model;
	Table *tab = createTable(model);

	tab->setDistribution(Table::DistHash, { "id", "region" });
	tab->setStorage(Table::StorageAoColumn, "zstd", 7);
	model.setServerFlavor(PgSqlVersions::FlavorCloudberry);

	QString xml = model.getSourceCode(SchemaParser::XmlCode);
	QVERIFY(xml.contains("server-flavor=\"cloudberry\""));
	QVERIFY(xml.contains("dist-policy=\"hash\""));
	QVERIFY(xml.contains("dist-keys=\"id,region\""));
	QVERIFY(xml.contains("storage=\"ao_column\""));
	QVERIFY(xml.contains("compress-level=\"7\""));
}

void MppTest::saveAndLoadModel()
{
	QTemporaryDir dir;
	QString filename = dir.filePath("mpp.dbm");

	{
		DatabaseModel model;
		Table *tab = createTable(model);

		tab->setDistribution(Table::DistHash, { "id", "region" });
		tab->setStorage(Table::StorageAoColumn, "zstd", 7);
		model.setServerFlavor(PgSqlVersions::FlavorGreenplum7);
		model.saveModel(filename, SchemaParser::XmlCode);
	}

	// Loading with a different flavor active to prove the file overrides it
	BaseObject::setServerFlavor(PgSqlVersions::FlavorPostgreSQL);

	DatabaseModel loaded;
	loaded.loadModel(filename);

	QCOMPARE(loaded.getServerFlavor(), PgSqlVersions::FlavorGreenplum7);
	QCOMPARE(BaseObject::getServerFlavor(), PgSqlVersions::FlavorGreenplum7);

	Table *tab = loaded.getTable("sch_mpp.sales");
	QVERIFY(tab != nullptr);
	QCOMPARE(tab->getDistributionPolicy(), Table::DistHash);
	QCOMPARE(tab->getDistributionColumns(), QStringList({ "id", "region" }));
	QCOMPARE(tab->getStorage(), Table::StorageAoColumn);
	QCOMPARE(tab->getCompressionType(), QString("zstd"));
	QCOMPARE(tab->getCompressionLevel(), 7u);

	QString sql = tab->getSourceCode(SchemaParser::SqlCode);
	QVERIFY(sql.contains("USING ao_column"));
	QVERIFY(sql.contains("DISTRIBUTED BY (id, region)"));
}

QTEST_MAIN(MppTest)
#include "mpptest.moc"
