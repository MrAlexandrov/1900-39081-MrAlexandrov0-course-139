import pytest
from .conftest import REGISTER_URL, create_multipart_data, create_headers_form_data  


async def test_register_missing_email(service_client):
    email = None
    password = "register_missing_email"
    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 400


async def test_register_missing_password(service_client):
    email = "register_missing_password@ya.ru"
    password = None
    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 400


async def test_register_missing_parametres(service_client):
    email = None
    password = None
    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 400


async def test_register_same_email(service_client):
    email = "register_same_email@ya.ru"
    password1 = "first_password"
    password2 = "second_password"
    data = await create_multipart_data(email=email, password=password1)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response

    data = await create_multipart_data(email=email, password=password2)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 409



async def test_register_success(service_client):
    email = "register_success@ya.ru"
    password = "register_success"
    data = await create_multipart_data(email=email, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert json_response["id"] is not None