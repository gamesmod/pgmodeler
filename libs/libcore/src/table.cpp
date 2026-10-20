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

#include "table.h"

Table::Table()
{
	obj_type = ObjectType::Table;
	with_oid=unlogged=rls_enabled=rls_forced=false;
	compress_level = 0;
	attributes[Attributes::Unlogged]="";
	attributes[Attributes::RlsEnabled]="";
	attributes[Attributes::RlsForced]="";
	attributes[Attributes::Oids]="";
	setName(tr("new_table"));
}

Table::~Table()
{
	destroyObjects();
}

void Table::setUnlogged(bool value)
{
	setCodeInvalidated(unlogged != value);
	unlogged=value;
}

void Table::setRLSEnabled(bool value)
{
	setCodeInvalidated(rls_enabled != value);
	rls_enabled = value;
}

void Table::setRLSForced(bool value)
{
	setCodeInvalidated(rls_forced != value);
	rls_forced = value;
}

void Table::addObject(BaseObject *object, int obj_idx)
{
	try
	{
		PhysicalTable::addObject(object, obj_idx);

		if(object->getObjectType() == ObjectType::Table)
		{
			/* Updating the storage parameter WITH OIDS depending on the ancestors.
			 * According to the docs, the child table will inherit WITH OID status from the parents */
			with_oid=(with_oid || dynamic_cast<Table *>(object)->isWithOIDs());
		}
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(),e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE,&e);
	}
}

void Table::removeObject(unsigned obj_idx, ObjectType obj_type)
{
	try
	{
		Table *tab = nullptr;
		PhysicalTable::removeObject(obj_idx, obj_type);
		with_oid=false;

		for(auto &obj : ancestor_tables)
		{
			tab = dynamic_cast<Table *>(obj);
			if(!with_oid && tab->isWithOIDs())
			{
				with_oid=true;
				break;
			}
		}
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(),e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE,&e);
	}
}

void Table::removeObject(const QString &name, ObjectType obj_type)
{
	try
	{
		PhysicalTable::removeObject(name, obj_type);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(),e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE,&e);
	}
}

void Table::removeObject(BaseObject *obj)
{
	try
	{
		PhysicalTable::removeObject(obj);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(),e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE,&e);
	}
}

std::vector<TableObject *> *Table::getObjectList(ObjectType obj_type)
{
	if(obj_type==ObjectType::Rule)
		return &rules;

	if(obj_type==ObjectType::Index)
		return &indexes;

	if(obj_type==ObjectType::Policy)
		return &policies;

	return PhysicalTable::getObjectList(obj_type);
}

void Table::addIndex(Index *ind, int idx)
{
	try
	{
		addObject(ind, idx);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

void Table::addRule(Rule *reg, int idx_reg)
{
	try
	{
		addObject(reg, idx_reg);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

void Table::addPolicy(Policy *pol, int idx_pol)
{
	try
	{
		addObject(pol, idx_pol);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
  }
}

void Table::removeIndex(const QString &name)
{
	try
	{
		removeObject(name,ObjectType::Index);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

void Table::removeIndex(unsigned idx)
{
	try
	{
		removeObject(idx,ObjectType::Index);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

void Table::removeRule(const QString &name)
{
	try
	{
		removeObject(name,ObjectType::Rule);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

void Table::removeRule(unsigned idx)
{
	try
	{
		removeObject(idx,ObjectType::Rule);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

void Table::removePolicy(const QString &name)
{
	try
	{
		removeObject(name, ObjectType::Policy);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

void Table::removePolicy(unsigned idx)
{
	try
	{
		removeObject(idx, ObjectType::Policy);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE, &e);
	}
}

Index *Table::getIndex(const QString &name)
{
	int idx;
	return dynamic_cast<Index *>(getObject(name,ObjectType::Index,idx));
}

Index *Table::getIndex(unsigned idx)
{
	return dynamic_cast<Index *>(getObject(idx,ObjectType::Index));
}

Rule *Table::getRule(const QString &name)
{
	int idx;
	return dynamic_cast<Rule *>(getObject(name,ObjectType::Rule,idx));
}

Rule *Table::getRule(unsigned idx)
{
	return dynamic_cast<Rule *>(getObject(idx,ObjectType::Rule));
}

Policy *Table::getPolicy(const QString &name)
{
	int idx;
	return dynamic_cast<Policy *>(getObject(name, ObjectType::Policy,idx));
}

Policy *Table::getPolicy(unsigned idx)
{
	return dynamic_cast<Policy *>(getObject(idx, ObjectType::Policy));
}

unsigned int Table::getObjectCount()
{
	return PhysicalTable::getObjectCount() +
				 indexes.size() +
				 rules.size() +
				 policies.size();
}

unsigned Table::getIndexCount()
{
	return indexes.size();
}

unsigned Table::getRuleCount()
{
	return rules.size();
}

unsigned Table::getPolicyCount()
{
	return policies.size();
}

void Table::getForeignKeys(std::vector<Constraint *> &fks, bool inc_added_by_rel, Table *ref_table)
{
	unsigned count,i;
	Constraint *constr=nullptr;

	count=constraints.size();
	for(i=0; i < count; i++)
	{
		constr=dynamic_cast<Constraint *>(constraints[i]);

		if(constr->getConstraintType()==ConstraintType::ForeignKey &&
				(!ref_table || (ref_table && constr->getReferencedTable()==ref_table)) &&
				(!constr->isAddedByLinking() ||
				 (constr->isAddedByLinking() && inc_added_by_rel)))
			fks.push_back(constr);
	}
}

bool Table::isUnlogged()
{
	return unlogged;
}

bool Table::isRLSEnabled()
{
	return rls_enabled;
}

bool Table::isRLSForced()
{
	return rls_forced;
}

void Table::setWithOIDs(bool value)
{
	setCodeInvalidated(with_oid != value);
	with_oid=value;
}

void Table::setDistribution(const QString &policy, const QStringList &columns)
{
	if(!policy.isEmpty() && policy != DistHash && policy != DistRandom && policy != DistReplicated)
	{
		throw Exception(Exception::getErrorMessage(ErrorCode::InvDistributionKeys)
										.arg(policy, getSignature()),
										ErrorCode::InvDistributionKeys, PGM_FUNC, PGM_FILE, PGM_LINE);
	}

	if((policy == DistHash && columns.isEmpty()) ||
		 (policy != DistHash && !columns.isEmpty()))
	{
		throw Exception(Exception::getErrorMessage(ErrorCode::InvDistributionKeys)
										.arg(policy.isEmpty() ? "-" : policy, getSignature()),
										ErrorCode::InvDistributionKeys, PGM_FUNC, PGM_FILE, PGM_LINE);
	}

	setCodeInvalidated(dist_policy != policy || dist_columns != columns);
	dist_policy = policy;
	dist_columns = columns;
}

QString Table::getDistributionPolicy()
{
	return dist_policy;
}

QStringList Table::getDistributionColumns()
{
	return dist_columns;
}

void Table::setStorage(const QString &stg, const QString &compression, unsigned level)
{
	setCodeInvalidated(storage != stg || compress_type != compression || compress_level != level);
	storage = stg;
	compress_type = compression;
	compress_level = level;
}

QString Table::getStorage()
{
	return storage;
}

QString Table::getCompressionType()
{
	return compress_type;
}

unsigned Table::getCompressionLevel()
{
	return compress_level;
}

bool Table::isWithOIDs()
{
	return with_oid;
}

bool Table::isReferTableOnForeignKey(Table *ref_tab)
{
	unsigned count,i;
	Constraint *constr=nullptr;
	bool found=false;

	count=constraints.size();
	for(i=0; i < count && !found; i++)
	{
		constr=dynamic_cast<Constraint *>(constraints[i]);
		found=(constr->getConstraintType()==ConstraintType::ForeignKey &&
			   !constr->isAddedByLinking() &&
			   constr->getReferencedTable() == ref_tab);
	}

	return found;
}

void Table::setMppAttributes(SchemaParser::CodeType def_type)
{
	attributes[Attributes::DistPolicy] = "";
	attributes[Attributes::DistKeys] = "";
	attributes[Attributes::Storage] = "";
	attributes[Attributes::CompressType] = "";
	attributes[Attributes::CompressLevel] = "";
	attributes[Attributes::AccessMethod] = "";
	attributes[Attributes::StorageOptions] = "";

	if(def_type == SchemaParser::XmlCode)
	{
		attributes[Attributes::DistPolicy] = dist_policy;
		attributes[Attributes::DistKeys] = dist_columns.join(',');
		attributes[Attributes::Storage] = storage;
		attributes[Attributes::CompressType] = compress_type;
		attributes[Attributes::CompressLevel] = compress_level ? QString::number(compress_level) : "";
		return;
	}

	// The MPP clauses are only generated for the flavors that support them
	if(!PgSqlVersions::isMppFlavor(PgSqlVersions::getServerFlavor()))
		return;

	QStringList options;

	if(dist_policy == DistHash)
	{
		QStringList cols;

		for(auto &col_name : dist_columns)
		{
			Column *col = getColumn(col_name);

			if(!col)
			{
				throw Exception(Exception::getErrorMessage(ErrorCode::InvDistributionKeyColumn)
												.arg(col_name, getSignature()),
												ErrorCode::InvDistributionKeyColumn, PGM_FUNC, PGM_FILE, PGM_LINE);
			}

			cols.append(col->getName(true));
		}

		attributes[Attributes::DistKeys] = cols.join(", ");
	}

	attributes[Attributes::DistPolicy] = dist_policy;

	if(PgSqlVersions::getServerFlavor() == PgSqlVersions::FlavorGreenplum6)
	{
		// Greenplum 6 has no table access methods: the storage is selected through reloptions
		if(storage == StorageAoRow || storage == StorageAoColumn)
		{
			options.append("appendonly=true");

			if(storage == StorageAoColumn)
				options.append("orientation=column");
		}
	}
	else if(!storage.isEmpty())
		attributes[Attributes::AccessMethod] = storage;

	if(!compress_type.isEmpty() && storage != StorageHeap)
		options.append(QString("compresstype=%1").arg(compress_type));

	if(compress_level > 0 && storage != StorageHeap)
		options.append(QString("compresslevel=%1").arg(compress_level));

	attributes[Attributes::StorageOptions] = options.join(", ");
}

QString Table::__getSourceCode(SchemaParser::CodeType def_type, bool incl_rel_added_objs, bool incl_constraints)
{
	setTableAttributes(def_type, incl_rel_added_objs, incl_constraints);

	attributes[Attributes::Oids]=(with_oid ? Attributes::True : "");
	attributes[Attributes::Unlogged]=(unlogged ? Attributes::True : "");
	attributes[Attributes::RlsEnabled]=(rls_enabled ? Attributes::True : "");
	attributes[Attributes::RlsForced]=(rls_forced ? Attributes::True : "");
	attributes[Attributes::CopyTable]="";

	setMppAttributes(def_type);

	if(def_type==SchemaParser::SqlCode && copy_table)
		attributes[Attributes::CopyTable]=copy_table->getName(true) + copy_op.getSQLDefinition();

	return BaseObject::__getSourceCode(def_type);
}

QString Table::getDataDictionary(bool split, bool md_format, const attribs_map &extra_attribs)
{
	try
	{
		attribs_map attribs = extra_attribs;

		for(auto &obj : indexes)
			attribs[Attributes::Indexes] +=  dynamic_cast<Index *>(obj)->getDataDictionary(md_format);

		return PhysicalTable::getDataDictionary(split, md_format, attribs);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(), e.getErrorCode(), PGM_FUNC, PGM_FILE, PGM_LINE, &e);
	}
}

void Table::updateDependencies()
{
	BaseTable::updateDependencies({});
}

QString Table::getSourceCode(SchemaParser::CodeType def_type)
{
	QString code_def=getCachedCode(def_type, false);
	if(!code_def.isEmpty()) return code_def;

	return __getSourceCode(def_type, false, true);
}

void Table::operator = (Table &tab)
{
	(*dynamic_cast<PhysicalTable *>(this))=dynamic_cast<PhysicalTable &>(tab);

	this->copy_op=tab.copy_op;
	this->unlogged=tab.unlogged;
	this->with_oid=tab.with_oid;
	this->rls_forced=tab.rls_forced;
	this->rls_enabled=tab.rls_enabled;
	this->dist_policy=tab.dist_policy;
	this->dist_columns=tab.dist_columns;
	this->storage=tab.storage;
	this->compress_type=tab.compress_type;
	this->compress_level=tab.compress_level;
}

QString Table::getTruncateDefinition(bool cascade)
{
	try
	{
		BaseObject::setBasicAttributes(true);
		attributes[Attributes::Cascade]=(cascade ? Attributes::True : "");
		return BaseObject::getAlterCode(Attributes::TruncatePriv, attributes, false, false);
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(),e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE,&e);
	}
}

QString Table::getAlterCode(BaseObject *object)
{
	Table *tab=dynamic_cast<Table *>(object);

	if(!tab)
		throw Exception(ErrorCode::OprNotAllocatedObject,PGM_FUNC,PGM_FILE,PGM_LINE);

	try
	{
		QString alter_def;
		attribs_map attribs;

		attribs[Attributes::Oids]="";
		attribs[Attributes::AlterCmds]=BaseObject::getAlterCode(object, true);

		if(this->getName()==tab->getName())
		{
			attribs[Attributes::HasChanges]=Attributes::True;

			if(this->with_oid!=tab->with_oid)
				attribs[Attributes::Oids]=(tab->with_oid ? Attributes::True : Attributes::Unset);

			if(this->unlogged!=tab->unlogged)
				attribs[Attributes::Unlogged]=(tab->unlogged ? Attributes::True : Attributes::Unset);

			if(this->rls_enabled!=tab->rls_enabled)
				attribs[Attributes::RlsEnabled]=(tab->rls_enabled ? Attributes::True : Attributes::Unset);

			if(this->rls_forced!=tab->rls_forced)
				attribs[Attributes::RlsForced]=(tab->rls_forced ? Attributes::True : Attributes::Unset);
		}

		copyAttributes(attribs);
		alter_def=BaseObject::getAlterCode(this->getSchemaName(), attributes, false, true);

		return alter_def;
	}
	catch(Exception &e)
	{
		throw Exception(e.getErrorMessage(),e.getErrorCode(),PGM_FUNC,PGM_FILE,PGM_LINE,&e);
	}
}
