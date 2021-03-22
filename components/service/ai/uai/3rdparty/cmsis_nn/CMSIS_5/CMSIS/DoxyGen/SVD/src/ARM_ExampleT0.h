/* ================                     TIMER0                     ================ */
typedef struct {                                    /*!< TIMER0 Structure                                                      */
  __IO uint32_t  CR;                                /*!< Control Register                                                      */
  __IO uint16_t  SR;                                /*!< Status Register                                                       */
  __I  uint16_t  RESERVED0[5];
  __IO uint16_t  INT;                               /*!< Interrupt Register                                                    */
  __I  uint16_t  RESERVED1[7];
  __IO uint32_t  COUNT;                             /*!< The Counter Register reflects the actual Value of the Timer/Counter   */
  __IO uint32_t  MATCH;                             /*!< The Match Register stores the compare Value for the MATCH condition   */
  
  union {
    __O  uint32_t  PRESCALE_WR;                     /*!< The Prescale Register stores the Value for the prescaler. The
                                                         cont event gets divided by this value                                 */
    __I  uint32_t  PRESCALE_RD;                     /*!< The Prescale Register stores the Value for the prescaler. The
                                                         cont event gets divided by this value                                 */
  };
  __I  uint32_t  RESERVED2[9];
  __IO uint32_t  RELOAD[4];                         /*!< The Reload Register stores the Value the COUNT Register gets
                                                         reloaded on a when a condition was met.                               */
} TIMER0_Type;
