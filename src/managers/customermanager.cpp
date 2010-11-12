#include "customermanager.h"

#include "QVariant"
#include "QSqlError"


Customer CustomerManager::get(int id)
{
    QSqlQuery query;

    query.prepare(
            "SELECT id, Name, Adress, Adress2, Zip, City, Phone, Mail "
            "FROM client WHERE id = :customer_id"
        );

    query.bindValue(":customer_id", QVariant(id));

    if(!query.exec()) {
        setError(query.lastError().databaseText(), query.lastQuery());

        return Customer();
    }

    // pas de client avec l'ID demandé, on ne remonte pas d'erreur
    // mais juste un client vide
    if(!query.next())
        return Customer();

    return makeCustomer(query);
}

bool CustomerManager::save(Customer &customer)
{
    return (customer.getId() == 0) ? insert(customer) : update(customer);
}

bool CustomerManager::erase(int id)
{
    QSqlQuery query;

    if(id == 0)
        return false;

    query.prepare("DELETE FROM client WHERE id = :c_id");

    query.bindValue(":c_id", QVariant(id));

    if(query.exec())
        return true;

    setError(query.lastError().databaseText(), query.lastQuery());

    return false;
}

bool CustomerManager::insert(Customer &customer)
{
    QSqlQuery query;

    query.prepare(
            "INSERT INTO client (Name, Adress, Adress2, Zip, City, Phone, "
                                     "Mail) "
            "VALUES (:name, :address, :address2, :zip, :city, :phone, :mail)"
    );

    bindCustomer(customer, query);

    if(query.exec())
    {
        customer.setId(query.lastInsertId().toInt());

        return true;
    }

    setError(query.lastError().databaseText(), query.lastQuery());

    return false;
}

bool CustomerManager::update(const Customer &customer)
{
    QSqlQuery query;

    query.prepare(
            "UPDATE user "
            "SET Name = :name, Adress = :address, Adress2 = :address2, "
                "Zip = :zip, City = :city, Phone = :phone, "
                "Mail = :mail, "
            "WHERE id = :c_id"
    );

    bindCustomer(customer, query);

    if(query.exec())
        return true;

    setError(query.lastError().databaseText(), query.lastQuery());

    return false;
}

void CustomerManager::bindCustomer(const Customer &customer, QSqlQuery &query)
{
    query.bindValue(":c_id", customer.getId());
    query.bindValue(":name", customer.getName());
    query.bindValue(":address", customer.getAddress());
    query.bindValue(":address2", customer.getAddress());
    query.bindValue(":zip", customer.getZipCode());
    query.bindValue(":city", customer.getCity());
    query.bindValue(":phone", customer.getPhone());
    query.bindValue(":mail", customer.getMail());
}

Customer CustomerManager::makeCustomer(QSqlQuery &query)
{
    Customer customer;

    customer.setId(query.value(0).toInt());
    customer.setName(query.value(1).toString());
    customer.setAddress(query.value(2).toString());
    customer.setAddressComplement(query.value(3).toString());
    customer.setZipCode(query.value(4).toString());
    customer.setCity(query.value(5).toString());
    customer.setPhone(query.value(6).toString());
    customer.setMail(query.value(7).toString());

    return customer;
}
