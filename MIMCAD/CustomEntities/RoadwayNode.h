#pragma once
#ifndef NODESOLID3D_H
#define NODESOLID3D_H

#define VERSION_NS 1 

#define WHICHSIDE int
#define STARTSIDE 1
#define ENDSIDE -1

#include <set>
#include <map>
#include <atomic>
#include "detail.h"

namespace MIM
{

	class Roadway;

	class RoadwayNodeReactor :public AcDbObjectReactor
	{
	public:
		ACRX_DECLARE_MEMBERS(RoadwayNodeReactor);

		RoadwayNodeReactor() {}
		virtual ~RoadwayNodeReactor() {};

		virtual void modified(const AcDbObject* dbObj) override;

		///virtual void erased(const AcDbObject* dbObj,Adesk::Boolean bErasing) override;

	};

	class __declspec(dllexport) RoadwayNode final:public AcDbEntity
	{
	private:
		typedef std::shared_ptr<RoadwayNodeReactor> pRoadwayNodeReactor;
		typedef std::map<AcDbHandle, WHICHSIDE> HdlsdMap;
		typedef std::shared_ptr<HdlsdMap> phdlsd;
		friend class RoadwayNodeReactor;


		enum ERoadwayNodeBuf
		{
			eInvalidHandle = 1,
			ePreHdlsd = 2
		};

		struct BufPair
		{
			ERoadwayNodeBuf type;
			Any buf;
		};
	public:
		ACRX_DECLARE_MEMBERS(RoadwayNode);

		RoadwayNode();
		virtual ~RoadwayNode();
	

	private:
		AcGePoint3d m_position;

		Adesk::Int32 m_radius;

		pRoadwayNodeReactor m_modified;

		phdlsd m_hdlsd;

		//�����ĳ��ʱ��������󲻿�д������Ҫ��ӻ���ɾ����roadway�����������
		std::vector<BufPair> preBuf;

		spin_mutex mutex;
		
	public:

		virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const override;

		virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler) override;

		//��дɾ���������Ʋ���ֱ��ɾ���ڵ�
		virtual Acad::ErrorStatus subErase(Adesk::Boolean erasing) override;

		virtual Acad::ErrorStatus subClose() override;


	public:


		virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw *mode) override;


		virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray &gripPoints,
			AcDbIntArray &osnapModes,
			AcDbIntArray &geomIds) const override;

		virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray &indices, const AcGeVector3d &offset) override;

		virtual Acad::ErrorStatus subGetOsnapPoints(
			AcDb::OsnapMode     osnapMode,
			Adesk::GsMarker     gsSelectionMark,
			const AcGePoint3d&  pickPoint,
			const AcGePoint3d&  lastPoint,
			const AcGeMatrix3d& viewXform,
			AcGePoint3dArray&   snapPoints,
			AcDbIntArray &   geomIds) const override;

		virtual Acad::ErrorStatus   subIntersectWith(
			const AcDbEntity*   ent,
			AcDb::Intersect     intType,
			const AcGePlane&    projPlane,
			AcGePoint3dArray&   points,
			Adesk::GsMarker     thisGsMarker = 0,
			Adesk::GsMarker     otherGsMarker = 0)
			const override;

		//ͨ��������������ж��Ƿ����������ཻ�ˣ��������������
		virtual Acad::ErrorStatus   subIntersectWith(
			const AcDbEntity*   ent,
			AcDb::Intersect     intType,
			AcGePoint3dArray&   points,
			Adesk::GsMarker     thisGsMarker = 0,
			Adesk::GsMarker     otherGsMarker = 0)
			const override;


		virtual Acad::ErrorStatus  subTransformBy(const AcGeMatrix3d& xform) override;

		virtual Acad::ErrorStatus subGetGeomExtents(AcDbExtents& extents)const override;

	public:


		void appendRoadway(AcDbHandle, WHICHSIDE);

		void removeRoadway(const AcDbHandle&);

		void clearRoadway() const;

		bool tryErase();

		Acad::ErrorStatus setPosition(const AcGePoint3d&);
		Acad::ErrorStatus setRadius(uint radius);

		inline const AcGePoint3d& getPosition() const;
		inline size_t getCountsOfBinded() const;

		inline const std::map<AcDbHandle, WHICHSIDE>& getHdlsd() const;

		void copyRoadway(const RoadwayNode &);

	private:
			void setPreBuf();
	};

}
#endif //NODESOLID3D_H

