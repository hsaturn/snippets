#pragma once

// Quick pseudo optional implementation (because my g++ version does not have it)

template<typename Type>
class Optional
{
	public:
		Optional() : mSet(false) { }
		Optional(Type value) : mSet(true), mValue(value) { }
		explicit operator bool() const { return mSet; }

		const Type& get() const
		{
			if (mSet) return mValue;
			throw string("Getting an undefined value");
		}

		template <typename T>
		friend ostream& operator<< (ostream& out, const Optional<T>&);

		Optional<Type> operator=(const Type value)
		{
			mSet = true; mValue = value; return *this;
		}

		Optional<Type> operator=(const Optional<Type>& opt)
		{
			mSet = opt.mSet; mValue = opt.value; return *this;
		}


	
	private:
		bool mSet;
		Type mValue;
};

