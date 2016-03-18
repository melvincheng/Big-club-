/*
 * This class holds data for individual accounts
 */
public class Account{
  private int id; // account number
  private String name; // account holder's full name
  private float balance; // account balance
  private boolean enabled; // flag of account if it is enabled
  private boolean student; // flag of account if it has a student plan

  /**
  * Constructor
  * all fields are set at construction of object
  */
  public Account(int id, String name, float balance, boolean enabled, boolean student){
    this.id = id;
    this.name = name;
    this.balance = balance;
    this.enabled = enabled;
    this.student = student;
  }


	/**
	* Returns value of id
	* @return
	*/
	public int getId() {
		return id;
	}

	/**
	* Sets new value of id
	* @param
	*/
	public void setId(int id) {
		this.id = id;
	}

	/**
	* Returns value of name
	* @return
	*/
	public String getName() {
		return name;
	}

	/**
	* Sets new value of name
	* @param
	*/
	public void setName(String name) {
		this.name = name;
	}

	/**
	* Returns value of balance
	* @return
	*/
	public float getBalance() {
		return balance;
	}

	/**
	* Sets new value of balance
	* @param
	*/
	public void setBalance(float balance) {
		this.balance = balance;
	}

	/**
	* Returns value of enabled
	* @return
	*/
	public boolean isEnabled() {
		return enabled;
	}

	/**
	* Sets new value of enabled
	* @param
	*/
	public void setEnabled(boolean enabled) {
		this.enabled = enabled;
	}

	/**
	* Returns value of student
	* @return
	*/
	public boolean isStudent() {
		return student;
	}

	/**
	* Sets new value of student
	* @param
	*/
	public void setStudent(boolean student) {
		this.student = student;
	}
}