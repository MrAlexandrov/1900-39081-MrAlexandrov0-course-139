import pytest
from .conftest import LOGIN_URL, create_multipart_data, create_headers_form_data


async def test_login_missing_email(service_client):
    email = None
    password = "login_missing_email"
    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 400


async def test_login_missing_password(service_client):
    email = "login_missing_password@ya.ru"
    password = None 
    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 400


async def test_login_missing_parametres(service_client):
    email = None
    password = None
    data = await create_multipart_data()
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 400


async def test_login_no_user(service_client):
    email = "login_no_user@ya.ru"
    password = "login_no_user"
    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 404


async def test_login_wrong_password(service_client, register_user):
    email = "login_wrong_password@ya.ru"
    password = "login_wrong_password"
    await register_user(email, password)

    data = await create_multipart_data(email=email, password="wrong password")
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 404


async def test_login_success(service_client, register_user):
    email = "login_success@ya.ru"
    password = "login_success"
    await register_user(email=email, password=password)

    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert json_response["id"] is not None