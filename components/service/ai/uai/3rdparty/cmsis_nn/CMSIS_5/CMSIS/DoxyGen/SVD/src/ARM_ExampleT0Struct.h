/* ================                     TIMER0                     ================ */
typedef struct {                                    /*!< TIMER0 Structure                                                      */
  
  union {
    __IO uint32_t  CR;                              /*!< Control Register                                                      */
    
    struct {
      __IO uint32_t  EN         :  1;               /*!< Enable                                                                */
      __O  uint32_t  RST        :  1;               /*!< Reset Timer                                                           */
      __IO uint32_t  CNT        :  2;               /*!< Counting direction                                                    */
      __IO uint32_t  MODE       :  3;               /*!< Operation Mode                                                        */
      __IO uint32_t  PSC        :  1;               /*!< Use Prescaler                                                         */
      __IO uint32_t  CNTSRC     :  4;               /*!< Timer / Counter Source Divider                                        */
      __IO uint32_t  CAPSRC     :  4;               /*!< Timer / Counter Capture Source                                        */
      __IO uint32_t  CAPEDGE    :  2;               /*!< Capture Edge, select which Edge should result in a counter increment
                                                         or decrement                                                          */
           uint32_t             :  2;
      __IO uint32_t  TRGEXT     :  2;               /*!< Triggers an other Peripheral                                          */
           uint32_t             :  2;
      __IO uint32_t  RELOAD     :  2;               /*!< Select RELOAD Register n to reload Timer on condition                 */
      __IO uint32_t  IDR        :  2;               /*!< Selects, if Reload Register number is incremented, decremented
                                                         or not modified                                                       */
           uint32_t             :  3;
      __IO uint32_t  S          :  1;               /*!< Starts and Stops the Timer / Counter                                  */
    } CR_b;                                         /*!< BitSize                                                               */
  };
  
  union {
    __IO uint16_t  SR;                              /*!< Status Register                                                       */
    
    struct {
      __I  uint16_t  RUN        :  1;               /*!< Shows if Timer is running or not                                      */
           uint16_t             :  7;
      __IO uint16_t  MATCH      :  1;               /*!< Shows if the MATCH was hit                                            */
      __IO uint16_t  UN         :  1;               /*!< Shows if an underflow occured. This flag is sticky                    */
      __IO uint16_t  OV         :  1;               /*!< Shows if an overflow occured. This flag is sticky                     */
           uint16_t             :  1;
      __I  uint16_t  RST        :  1;               /*!< Shows if Timer is in RESET state                                      */
           uint16_t             :  1;
      __I  uint16_t  RELOAD     :  2;               /*!< Shows the currently active RELOAD Register                            */
    } SR_b;                                         /*!< BitSize                                                               */
  };
  __I  uint16_t  RESERVED0[5];
  
  union {
    __IO uint16_t  INT;                             /*!< Interrupt Register                                                    */
    
    struct {
      __IO uint16_t  EN         :  1;               /*!< Interrupt Enable                                                      */
           uint16_t             :  3;
      __IO uint16_t  MODE       :  3;               /*!< Interrupt Mode, selects on which condition the Timer should
                                                         generate an Interrupt                                                 */
    } INT_b;                                        /*!< BitSize                                                               */
  };
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
