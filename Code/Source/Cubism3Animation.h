#pragma once

#include "Cubism3Debug.h"
#include "Cubism3Assets.h"
//#include "Cubism3EditorData.h"
#include "Include\Cubism3\Cubism3UIBus.h"

namespace Cubism3 {
	class ModelParametersGroup;
	class ModelPartsGroup;

	namespace FloatBlend {
		float Default(float base, float value, float weight);
		float Additive(float base, float value, float weight);
	}

	class Cubism3Animation {
	public:
		AZ_RTTI(Cubism3Animation, "{C80B4CDB-8FB4-4E10-B098-6B88213B9F4A}");

	public:
		Cubism3Animation();
		~Cubism3Animation();

	public:
		void Load(MotionAssetRef asset);

	public:
		bool Loaded() { return this->m_loaded; }

	public:
		void SetParametersAndParts(ModelParametersGroup * paramGroup, ModelPartsGroup * partsGroup);

	public:
		void SetFloatBlend(Cubism3AnimationFloatBlend floatBlendFunc) { this->m_floatBlendFunc = floatBlendFunc; }
		void SetWeight(float weight) { this->m_weight = weight; }
		float GetWeight() { return this->m_weight; }

	public:
		bool IsPlaying();
		bool IsStopped();
		bool IsPaused();
		bool IsLooping() { return this->m_meta.loop; }

	public:
		void Play();
		void Stop();
		void Pause();
		void SetLooping(bool loop);

	public:
		void Tick(float delta);
		void Reset();

	private:
		void UpdateCurves();

	private:
		bool m_loaded;
		ModelParametersGroup * m_paramGroup;
		ModelPartsGroup * m_partsGroup;

		Cubism3AnimationFloatBlend m_floatBlendFunc;

		float m_time;
		float m_weight;
		bool m_playedOnce;

		bool m_playing;

	private: //meta data
		struct Meta {
			float duration; //end time
			float fps;
			bool loop;
			unsigned int curveCount;
			unsigned int totalSegCount;
			unsigned int totalPointCount;
		};

		Meta m_meta;

	private: //Curve stuff
		enum SegmentType {
			LINEAR = 0,
			BEZIER,
			STEPPED,
			ISTEPPED
		};

		class Calc {
		public:
			virtual float calculate(float time) = 0;
			virtual SegmentType getType() = 0;
		};

		class Linear : public Calc {
		public:
			AZStd::pair<float, float> data[2];
			float calculate(float time);
			SegmentType getType() { return LINEAR; }
		};

		class Bezier : public Calc {
		public:
			AZStd::pair<float, float> data[4];
			float calculate(float time);
			SegmentType getType() { return BEZIER; }
		};

		class Stepped : public Calc {
		public:
			SegmentType type;
			AZStd::pair<float, float> data;
			float calculate(float time) { return this->data.second; }
			SegmentType getType() { return type; }
		};

		enum CurveTarget {
			Model,
			Parameter,
			Part
		};

		struct Curve {
			CurveTarget target;
			int id; //id from m_group - -1 for model opacity
			AZStd::string idStr;
			AZStd::vector<AZStd::pair<float, Calc *>> segments; //keyframe -> calc //reverse lookup, if time > keyframe then calc(time)
		};

		AZStd::vector<Curve *> m_curves;

	public: //RTTI stuff
		static void Reflect(AZ::SerializeContext* serializeContext);
	};
}