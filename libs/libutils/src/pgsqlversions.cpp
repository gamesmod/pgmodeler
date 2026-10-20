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

#include "pgsqlversions.h"
#include "exception.h"

namespace PgSqlVersions {
	static QString server_flavor { FlavorPostgreSQL };

	QString getFlavorName(const QString &flavor)
	{
		if(flavor == FlavorGreenplum6)
			return "Greenplum 6";

		if(flavor == FlavorGreenplum7)
			return "Greenplum 7";

		if(flavor == FlavorCloudberry)
			return "Apache Cloudberry";

		return "PostgreSQL";
	}

	QString getFlavorBaseVersion(const QString &flavor)
	{
		if(flavor == FlavorGreenplum6)
			return PgSqlVersion94;

		if(flavor == FlavorGreenplum7)
			return PgSqlVersion120;

		if(flavor == FlavorCloudberry)
			return PgSqlVersion140;

		return "";
	}

	bool isMppFlavor(const QString &flavor)
	{
		return flavor == FlavorGreenplum6 ||
					 flavor == FlavorGreenplum7 ||
					 flavor == FlavorCloudberry;
	}

	void setServerFlavor(const QString &flavor)
	{
		server_flavor = AllFlavors.contains(flavor) ? flavor : FlavorPostgreSQL;
	}

	QString getServerFlavor()
	{
		return server_flavor;
	}

	QString parseString(const QString &pgsql_ver, bool ignore_legacy_ver)
	{
		unsigned curr_ver = QString(pgsql_ver).remove('.').toUInt(),
				min_ver = QString(MinimumVersion).remove('.').toUInt(),
				default_ver = QString(DefaulVersion).remove('.').toUInt();

		/* Greenplum 6 is based on PostgreSQL 9.4 so versions below the
		 * minimum one are valid when generating code for it */
		if(server_flavor == FlavorGreenplum6)
			ignore_legacy_ver = true;

		if(!ignore_legacy_ver && curr_ver != 0 && (curr_ver < min_ver))
		{
			throw Exception(Exception::getErrorMessage(ErrorCode::InvPostgreSQLVersion)
											.arg(pgsql_ver, PgSqlVersions::MinimumVersion, PgSqlVersions::DefaulVersion),
											ErrorCode::InvPostgreSQLVersion, PGM_FUNC, PGM_FILE, PGM_LINE);
		}

		if(curr_ver > 0)
		{
			// If valid version just returns the version
			if(curr_ver <= default_ver)
				return pgsql_ver;

			/* If it's a new version greater than the default
			 * we fallback to the default one */
			if(curr_ver > default_ver)
				return DefaulVersion;
		}

		return PgSqlVersions::MinimumVersion;
	}
}
