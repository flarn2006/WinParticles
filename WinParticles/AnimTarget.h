#pragma once

template <typename TValue>
class CAnimTarget
{
public:
	class CValueAccessor;

private:
	CValueAccessor *accessor;

public:
	CAnimTarget()
	{
		accessor = new CValueAccessor(this);
	}

	~CAnimTarget()
	{
		delete accessor;
	}

	virtual TValue GetValue() = 0;
	virtual void SetValue(TValue newValue) = 0;

	CValueAccessor &operator*()
	{
		return *accessor;
	}

	class CValueAccessor
	{
	private:
		CAnimTarget<TValue> *animTarget;

	public:
		CValueAccessor(CAnimTarget<TValue> *animTarget)
		{
			this->animTarget = animTarget;
		}

		operator TValue()
		{
			return animTarget->GetValue();
		}

		CValueAccessor &operator=(TValue newValue)
		{
			animTarget->SetValue(newValue);
		}
	};
};

