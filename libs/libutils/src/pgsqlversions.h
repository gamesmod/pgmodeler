/*
# PostgreSQL Database Modeler (pgModeler)
#
# (c) Copyright 2006-2026 - Raphael Araújo e Silva <raphael@pgmodeler.io>
#
# DEVELOPMENT, MAINTENANCE AND COMMERCIAL DISTRIBUTION BY:
# Nullptr Labs Software e Tecnologia LTDA <contact@nullptrlabs.io>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# The complete text of GPLv3 is at LICENSE file on source code root directory.
# Also, you can get the complete GNU General Public License at <http://www.gnu.org/licenses/>
*/

/**
\ingroup libutils
\namespace PgSQLVersions
\brief Definition of PgSQLVersions namespace wich stores a series of static strings constants used
 to reference the supported PostgreSQL version.
*/

#ifndef PGSQL_VERSIONS_H
#define PGSQL_VERSIONS_H

#include "utilsglobal.h"
#include <QStringList>

namespace PgSqlVersions {
	inline const QString
	PgSqlVersion94("9.4"),
	PgSqlVersion100("10.0"),
	PgSqlVersion110("11.0"),
	PgSqlVersion120("12.0"),
	PgSqlVersion130("13.0"),
	PgSqlVersion140("14.0"),
	PgSqlVersion150("15.0"),
	PgSqlVersion160("16.0"),
	PgSqlVersion170("17.0"),
	PgSqlVersion180("18.0"),
	DefaulVersion = PgSqlVersion180,
	MinimumVersion = PgSqlVersion100;

	inline const QStringList
	AllVersions = {
		PgSqlVersion180, PgSqlVersion170, PgSqlVersion160,
		PgSqlVersion150, PgSqlVersion140, PgSqlVersion130,
		PgSqlVersion120, PgSqlVersion110, PgSqlVersion100
	};

	/*! \brief Server flavors. Besides vanilla PostgreSQL, gpModeler can generate code
	 * for MPP engines derived from it. Each flavor is mapped to the version of PostgreSQL
	 * it is based on, which is the one used to select version dependent syntax */
	inline const QString
	FlavorPostgreSQL("postgresql"),
	FlavorGreenplum6("greenplum6"), // PostgreSQL 9.4 based
	FlavorGreenplum7("greenplum7"), // PostgreSQL 12 based
	FlavorCloudberry("cloudberry"); // PostgreSQL 14 based

	inline const QStringList
	AllFlavors = { FlavorPostgreSQL, FlavorGreenplum6, FlavorGreenplum7, FlavorCloudberry };

	//! \brief Returns the human readable name of the flavor
	__libutils QString getFlavorName(const QString &flavor);

	//! \brief Returns the PostgreSQL version the flavor is based on (empty for FlavorPostgreSQL)
	__libutils QString getFlavorBaseVersion(const QString &flavor);

	//! \brief Returns true when the flavor is a MPP engine (Greenplum or Cloudberry)
	__libutils bool isMppFlavor(const QString &flavor);

	/*! \brief Defines the flavor used in code generation. An invalid value falls back to FlavorPostgreSQL.
	 * The versions below MinimumVersion are accepted by parseString only when the flavor is Greenplum 6 */
	__libutils void setServerFlavor(const QString &flavor);
	__libutils QString getServerFlavor();

	/*! \brief Parses and checks a string contaning a PostgreSQL version in the form [major.minor]
	 * When the flag ignore_legacy_ver is true, this method will raise an error if an invalid version
	 * is provided. An invalid version is considered any string in incorrect format or a valid string
	 * but refers to an obsolete version (< 10.0). If an invalid major version is proveded then the function
	 * will return the DefaultVersion as a fallback. */
	__libutils QString parseString(const QString &pgsql_ver, bool ignore_legacy_ver);
}

#endif
