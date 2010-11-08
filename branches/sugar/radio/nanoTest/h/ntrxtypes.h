/**
 * @file ntrxtypes.h
 * @date 2007-Dez-11
 * @author S. Rohdemann
 * @c (C) 2007 Nanotron Technologies
 * @brief Type definitions for data types.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the generic datatype definitions of the nTRX driver.
 *    These type definitions isolate the nTRX code from dependencies on the
 *    hardware environment for which the nTRX is compiled.
 */

/*
 * $Log$
 */
#ifndef	_NTRXTYPES_H
#define	_NTRXTYPES_H
#define	BITS2BYTES(n)	(n/8)		/* calc. bits into bytes */

#ifndef	NULL
#define	NULL	(0)
#endif	/* NULL */

#ifndef	NIL
#define	NIL	(0)
#endif	/* NIL */

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Signed character at least 8 bits in length. */
typedef	char	MyChar8T;

/** @brief Unsigned character at least 8 bits in length. */
typedef	unsigned	char	MyByte8T;

/** @brief Signed integer at least 16 bits in length. */
typedef	signed	short	MyInt16T;

/** @brief Unsigned integer at least 16 bits in length. */
typedef	unsigned	short	MyWord16T;

/** @brief Signed long at least 32 bits in length. */
typedef	signed	long	MyLong32T;

/** @brief Unsigned long at least 32 bits in length. */
typedef	unsigned	long	MyDword32T;

/** @brief double at least 32 bits in length. */
typedef	double	MyDouble32T;

#ifdef	TRUE
#	undef	TRUE
#endif	/* TRUE */
#ifdef	FALSE
#	undef	FALSE
#endif	/* FALSE */

/** @brief Bool type. */
typedef	enum	{
	FALSE = 0,
	TRUE = 1
}	MyBoolT;

/** @brief Void pointer type. */
typedef	void	*MyPtrT;

/** @brief Void function pointer type. */
typedef	void	(*MyFnPtrT)(void);

/** @brief Stores the time in seconds. */
typedef	MyDword32T	MyTimeT;

/** @brief Stores the time in microseconds. */
typedef	MyDword32T	MyMtimeT;

/** @brief 48 bit address type. */
typedef	MyByte8T	MyAddressT[BITS2BYTES(48)];
/** @brief 48 bit address type. */
typedef	MyAddressT	MyAddrT;

/** @brief 24 bit data type. Needed for Logical Netword Identification LIND. */
typedef	MyByte8T	MyByte24T[BITS2BYTES(24)];

/** @brief 64 bit data type. Needed for Sync Word. */
typedef	MyByte8T	MyByte64T[BITS2BYTES(64)];

/**
 * @brief General purpose message structure.
 *
 * This general purpose message structure is used for all
 * layer to layer communication.
 */
typedef struct
{
	MyByte8T prim;			/**< The primitive of the message. */
	MyAddrT  addr;			/**< MAC address of the message. */
	MyAddrT  rxAddr;		/**< MAC address of the recipient */
	MyByte8T len;			/**< Payload length. */
	MyByte8T data[128];	/**< Payload of the message. */

	MyByte8T status;		/**< Status value for confirmations */
	MyWord16T value;		/**< Configuration value of a layer setting */
	MyByte8T attribute;
#	ifdef CONFIG_NTRX_SNIFFER
	MyDword32T count;		/**< Sniffer stuff */
	MyByte8T frameType;	/**< Frame type of the message */
	MyByte8T extBits;		/**< Extended bits in the header for ranging */
#	endif /* CONFIG_NTRX_SNIFFER */
} MyMsgT;

/**
 * @brief Timer structure used by the timerlib.
 *
 * This structure is equal to the clib timer structure.
 */
typedef	struct
{
	MyInt16T	tm_sec;
	MyInt16T	tm_min;
	MyInt16T	tm_hour;
	MyInt16T	tm_mday;
	MyInt16T	tm_mon;
	MyInt16T	tm_year;
	MyInt16T	tm_wday;
	MyInt16T	tm_yday;
	MyInt16T	tm_isdst;
}	MyTmT;


#ifdef __cplusplus
}
#endif

#endif	/* _NTRXTYPES_H */
